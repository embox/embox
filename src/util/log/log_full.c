/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <stdarg.h>
#include <stdint.h>

#include "log.h"

void log_full_handle(struct logger *logger, uint16_t flags, const char *fmt,
    ...) {
	log_handler_t handler;
	va_list args;

	if ((handler = log_get_handler()) && (logger->level >= LOG_PRIO(flags))) {
		va_start(args, fmt);
		handler(logger, flags, fmt, args);
		va_end(args);
	}
}
