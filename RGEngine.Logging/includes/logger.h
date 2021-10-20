#pragma once

#pragma once

#include "core/error_codes.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

struct logger_event
{
	va_list ap;
	const char* fmt;
	const char* file;
	struct tm* time;
	void* udata;
	int line;
	int level;
};

typedef void (*logger_log_fn)(struct logger_event* event);
typedef void (*logger_lock_fn)(bool should_lock, void* data);

enum log_levels { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

/// <summary>
/// gets the log level name of specified level
/// </summary>
/// <param name="level">: to get the name from</param>
/// <returns>pointer to initialize</returns>
const char* logger_level_string(enum log_levels level);

/// <summary>
/// sets a lock function to lock specified data when calling from multiple threads
/// </summary>
/// <param name="callback">: function, called to lock specified data when logging</param>
/// <param name="data">: to lock, passed to callback when called</param>
void logger_set_lock(logger_lock_fn callback, void* data);

/// <summary>
/// sets log level, any logging call with this or higher level will be logged, anything lower will be ignored
/// </summary>
/// <param name="level">: to set</param>
void logger_set_level(enum log_levels level);

/// <summary>
/// if quiet prevents logging to stderr
/// </summary>
/// <param name="enable">: quiet or not</param>
void logger_set_quiet(bool enable);

/// <summary>
/// adds callback to be called when logging
/// </summary>
/// <param name="callback">: function to be called</param>
/// <param name="data">:  to log, passed to callback</param>
/// <param name="level">:  of when to call the callback</param>
/// <returns>error code or SUCCESS</returns>
enum error_codes logger_add_callback(logger_log_fn callback, void* data, enum log_levels level);

/// <summary>
/// adds file to log to when logging
/// </summary>
/// <param name="file">: pointer to log to</param>
/// <param name="level">:  of when to log to the file</param>
/// <returns>error code or SUCCESS</returns>
enum error_codes logger_add_file(FILE* file, enum log_levels level);

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
#define log_trace(...) logger_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define log_debug(...) logger_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define log_info(...)  logger_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define log_warn(...)  logger_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define log_error(...) logger_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// utility log macro calls log_log with predefined level, file and line
/// </summary>
/// <param name="...">: variadic parameter to pass log string and values to pass to log string</param>
#define log_fatal(...) logger_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)