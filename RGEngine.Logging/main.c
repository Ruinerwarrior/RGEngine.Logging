#include <RGEngine/logger.h>

int main()
{	
	logger_init(LEVEL_DEBUG);
	logger_add_console_sink(stderr, LEVEL_DEBUG);
	LOG_DEBUG("test");
	LOG_TRACE("haha");
}