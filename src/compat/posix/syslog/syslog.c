/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <stdarg.h>
#include <syslog.h>

void syslog(int priority, const char *message, ...) {
	va_list args;

	va_start(args, message);
	vsyslog(priority, message, args);
	va_end(args);
}
