![example workflow](https://github.com/Ruinerwarrior/RGEngine.Logging/actions/workflows/windows.yml/badge.svg)
![example workflow](https://github.com/Ruinerwarrior/RGEngine.Logging/actions/workflows/macOs.yml/badge.svg)
![example workflow](https://github.com/Ruinerwarrior/RGEngine.Logging/actions/workflows/ubuntu.yml/badge.svg)

# RGEngine.Logging
Simple logging library of RGEngine

## Features
- Multiple sinks to log to (files, console)
- Filter logging according to log level
- ANSI colouring of different log levels to console

## Examples
### Create console logger
With this example all log calls above ```LEVEL_TRACE``` will be logged to stderr.
```c
#include "RGEngine/logger.h"

int main()
{
    // only log calls above or equal given level will be logged 
    logger_init(LEVEL_TRACE);
    
    // sets output stream and specific log level for specified output stream, only log calls 
    // above or equal the previous set log level and the given log level will be logged to this stream
    logger_add_console_sink(stderr, LEVEL_TRACE);
	
    // log text to specified sinks
    LOG_TRACE("test: %i", 1);
    LOG_DEBUG("test: %i", 2);
    LOG_INFO("test: %i", 3);
    LOG_WARN("test: %i", 4);
    LOG_ERROR("test: %i", 5);
    LOG_FATAL("test: %i", 6);
}
```

### Create console and file logger
```c
#include "RGEngine/logger.h"

int main() 
{
	// file to log to
	FILE* file = fopen("./test.log", "w");

	// only log calls above or equal given level will be logged 
	logger_init(LEVEL_TRACE);

	// sets output stream and specific log level for specified output stream, only log calls 
	// above or equal the previous set log level and the given log level will be logged to this stream
	logger_add_console_sink(stderr, LEVEL_TRACE);

	// sets output stream and specific log level for specified file stream, only log calls 
	// above or equal the previous set log level and the given log level will be logged to this stream
	logger_add_file_sink(file, LEVEL_ERROR);

	// log text to specified sinks
	// logs to console
	LOG_TRACE("test: %i", 1);
	LOG_DEBUG("test: %i", 2);
	LOG_INFO("test: %i", 3);
	LOG_WARN("test: %i", 4);

	// logs to console and file
	LOG_ERROR("test: %i", 5);
	LOG_FATAL("test: %i", 6);
}
```
