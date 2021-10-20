#include "logger.h"

#define MAX_CALLBACKS 32

struct callback
{
	logger_log_fn fn;
	void* udata;
	enum log_levels level;
};

static struct
{
	void* udata;
	logger_lock_fn lock;
	enum log_levels level;
	bool quiet;
	struct callback callbacks[MAX_CALLBACKS];
} L;

static const char* level_strings[] = {
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef LOGGER_USE_COLOR
static const char* level_colors[] = {
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif

static void stdout_callback(struct logger_event* event)
{
	char buf[16];
	buf[strftime(buf, sizeof(buf), "%H:%M:%S", event->time)] = '\0';
#ifdef LOG_USE_COLOR
	fprintf(
		event->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
		buf, level_colors[event->level], level_strings[event->level],
		event->file, event->line);
#else
	fprintf(
		event->udata, "%s %-5s %s:%d: ",
		buf, level_strings[event->level], event->file, event->line);
#endif
	vfprintf(event->udata, event->fmt, event->ap);
	fprintf(event->udata, "\n");
	fflush(event->udata);
}

static void file_callback(struct logger_event* event)
{
	char buf[64];
	buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", event->time)] = '\0';
	fprintf(
		event->udata, "%s %-5s %s:%d: ",
		buf, level_strings[event->level], event->file, event->line);
	vfprintf(event->udata, event->fmt, event->ap);
	fprintf(event->udata, "\n");
	fflush(event->udata);
}

static void lock(void)
{
	if (L.lock) { L.lock(true, L.udata); }
}

static void unlock(void)
{
	if (L.lock) { L.lock(false, L.udata); }
}

const char* logger_level_string(enum log_levels level)
{
	return level_strings[level];
}

void logger_set_lock(logger_lock_fn fn, void* udata)
{
	L.lock = fn;
	L.udata = udata;
}

void logger_set_level(enum log_levels level)
{
	L.level = level;
}

void logger_set_quiet(bool enable)
{
	L.quiet = enable;
}

enum error_codes log_add_callback(logger_log_fn callback, void* data, enum log_levels level)
{
	for (int i = 0; i < MAX_CALLBACKS; i++)
	{
		if (!L.callbacks[i].fn)
		{
			L.callbacks[i] = (struct callback){ callback, data, level };
			return SUCCESS;
		}
	}
	return ERROR_MAX_LOG_CALLBACKS;
}

enum error_codes logger_add_file(FILE* fp, enum log_levels level)
{
	return logger_add_callback(file_callback, fp, level);
}

static void init_event(struct logger_event* event, void* udata)
{
	if (!event->time)
	{
		time_t t = time(NULL);
		event->time = localtime(&t);
	}
	event->udata = udata;
}

void logger_log(enum log_levels level, const char* file, int32_t line, const char* fmt, ...)
{
	struct logger_event event = {
	  .fmt = fmt,
	  .file = file,
	  .line = line,
	  .level = level,
	};

	lock();

	if (!L.quiet && level >= L.level)
	{
		init_event(&event, stderr);
		va_start(event.ap, fmt);
		stdout_callback(&event);
		va_end(event.ap);
	}

	for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++)
	{
		struct callback* cb = &L.callbacks[i];
		if (level >= cb->level)
		{
			init_event(&event, cb->udata);
			va_start(event.ap, fmt);
			cb->fn(&event);
			va_end(event.ap);
		}
	}

	unlock();
}
