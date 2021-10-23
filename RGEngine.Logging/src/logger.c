#include "logger.h"

typedef void (*logger_log_fn)(struct logger_event* event);

static const char* level_strings[] = {
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef LOGGER_USE_COLOR
static const char* level_colors[] = {
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif

static struct
{
	struct sink console_sinks[MAX_LOG_SINKS];
	struct sink file_sinks[MAX_LOG_SINKS];
	struct tm time;
	logger_log_fn log_function;
	logger_lock_fn lock_function;
	logger_unlock_fn unlock_function;

	uint16_t console_sink_count;
	uint16_t file_sink_count;
	enum log_levels level;
} log;

static void on_log_to_console(struct logger_event* event)
{
	char buf[16];
	buf[strftime(buf, sizeof(buf), "%H:%M:%S", &event->time)] = '\0';
#ifdef LOGGER_USE_COLOR
	fprintf(
		event->data,
		"%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
		buf,
		level_colors[event->level],
		level_strings[event->level],
		event->file,
		event->line
	);
#else
	fprintf(
		event->udata,
		"%s %-5s %s:%d: ",
		buf,
		level_strings[event->level],
		event->file,
		event->line
	);
#endif
	vfprintf(event->data, event->fmt, event->arguments);
	fprintf(event->data, "\n");
	fflush(event->data);
}

static void on_log_to_file(struct logger_event* event)
{
	char buf[64];
	buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &event->time)] = '\0';
	fprintf(
		event->data,
		"%s %-5s %s:%d: ",
		buf,
		level_strings[event->level],
		event->file, 
		event->line
	);
	vfprintf(event->data, event->fmt, event->arguments);
	fprintf(event->data, "\n");
	fflush(event->data);
}

static void on_log(struct logger_event* event)
{
	if (event->level >= log.level)
	{
		for (int i = 0; i < log.console_sink_count; i++)
		{
			struct sink* sink = &log.console_sinks[i];

			if (event->level >= sink->level)
			{
				event->data = sink->data;
				on_log_to_console(event);
			}
		}

		for (int j = 0; j < log.file_sink_count; j++)
		{
			struct sink* sink = &log.file_sinks[j];

			if (event->level >= sink->level)
			{
				event->data = sink->data;
				on_log_to_file(event);
			}
		}
	}
}

static void on_log_thread_safe(struct logger_event* event)
{
	if (event->level >= log.level)
	{
		for (int i = 0; i < log.console_sink_count; i++)
		{
			log.lock_function(log.console_sinks[i].data);

			struct sink* sink = &log.console_sinks[i];

			if (event->level >= sink->level)
			{
				event->data = sink->data;
				on_log_to_console(event);
			}

			log.unlock_function(log.console_sinks[i].data);
		}

		for (int j = 0; j < log.file_sink_count; j++)
		{
			log.lock_function(log.file_sinks[j].data);

			struct sink* sink = &log.file_sinks[j];

			if (event->level >= sink->level)
			{
				event->data = sink->data;
				on_log_to_file(event);
			}

			log.unlock_function(log.file_sinks[j].data);
		}
	}
}

void logger_init(enum log_levels level)
{
	log.level = level;
	log.log_function = on_log;
	log.lock_function = NULL;
	log.unlock_function = NULL;
	log.console_sink_count = 0;
	log.file_sink_count = 0;
}

void logger_init_threaded(enum log_levels level, logger_lock_fn lock_fn, logger_unlock_fn unlock_fn)
{
	log.level = level;
	log.log_function = on_log_thread_safe;
	log.lock_function = lock_fn;
	log.unlock_function = unlock_fn;
	log.console_sink_count = 0;
	log.file_sink_count = 0;
}

enum error_codes logger_add_file_sink(FILE* file, enum log_levels level)
{
	if (!file)
		return ERROR_FILE_IS_NULL;

	if (log.file_sink_count == MAX_LOG_SINKS)
		return ERROR_TO_MANY_SINKS;

	log.file_sinks[log.file_sink_count].level = level;
	log.file_sinks[log.file_sink_count].data = file;
	log.file_sink_count++;
}

enum error_codes logger_add_console_sink(void* data, enum log_levels level)
{
	if (!data)
		return ERROR_PTR_IS_NULL;

	if (log.file_sink_count == MAX_LOG_SINKS)
		return ERROR_TO_MANY_SINKS;

	log.console_sinks[log.console_sink_count].level = level;
	log.console_sinks[log.console_sink_count].data = data;
	log.console_sink_count++;
}

void logger_log(enum log_levels level, const char* file, int32_t line, const char* fmt, ...)
{
	struct logger_event event = {
	.fmt = fmt,
	.file = file,
	.line = line,
	.level = level,
	};

// fix warning(C4996) on WIN32
#ifdef WIN32
	time_t t = time(NULL);
	localtime_s(&event.time, &t);
#else
	time_t t = time(NULL);
	event.time = *localtime(&t);
#endif // WIN32

	va_start(event.arguments, fmt);
	log.log_function(&event);
	va_end(event.arguments);
}