#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#ifndef MAX_LOG_SINKS
#define MAX_LOG_SINKS 8
#endif // !MAX_LOG_SINKS

#ifdef __cplusplus
extern "C" {
#endif

enum log_levels { LEVEL_TRACE, LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR, LEVEL_FATAL };
enum error_codes { SUCCESS, ERROR_TO_MANY_SINKS, ERROR_FILE_IS_NULL, ERROR_PTR_IS_NULL };

struct logger_event
{
	va_list arguments;
	const char* fmt;
	const char* file;
	void* data;
	struct tm time;
	int32_t level;
	int32_t line;
};

//typedef void (*logger_log_fn)(struct logger_event* event);
typedef void (*logger_lock_fn)(void* data);
typedef void (*logger_unlock_fn)(void* data);

struct sink
{
	void* data;
	enum log_levels level;
};

/// <summary>
/// initialize logger, should only be called once and not be used for multithreaded code
/// </summary>
/// <param name="level">: of when to call the callback</param>
void logger_init(enum log_levels level);

/// <summary>
/// initialize logger, should only be called once should be used for multithreaded code
/// </summary>
/// <param name="level">: of when to call the callback</param>
void logger_init_threaded(enum log_levels level, logger_lock_fn lock_fn, logger_unlock_fn unlock_fn);

enum error_codes logger_add_console_sink(void* data, enum log_levels level);
enum error_codes logger_add_file_sink(FILE* file, enum log_levels level);
enum error_codes logger_set_level(enum log_levels level);

/// <summary>
/// log function, calls all callbacks and logs to stdout
/// </summary>
/// <param name="level">: of when to call the callback</param>
/// <param name="file">: name of log origin</param>
/// <param name="line">: number of log origin</param>
/// <param name="fmt">: log string</param>
/// <param name="...">: variadic parameter to pass to formated string</param>
void logger_log(enum log_levels level, const char* file, int32_t line, const char* fmt, ...);

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define LOG_TRACE(...) logger_log(LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define LOG_DEBUG(...) logger_log(LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define LOG_INFO(...)  logger_log(LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define LOG_WARN(...)  logger_log(LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define LOG_ERROR(...) logger_log(LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define LOG_FATAL(...) logger_log(LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif