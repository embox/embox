/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <util/log.h>

void __log_handle_static(int flags, const char *fmt, ...) {
	log_handler_t handler;
	va_list args;

	if ((handler = log_get_handler())) {
		va_start(args, fmt);
		handler(NULL, flags, fmt, args);
		va_end(args);
	}
}
