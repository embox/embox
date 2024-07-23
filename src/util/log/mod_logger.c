/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>

#include <util/log.h>

void __log_handle_mod(struct mod_logger *logger, int flags, const char *fmt,
    ...) {
	log_handler_t handler;
	va_list args;

	assert(logger);

	handler = log_get_handler();

	if (handler && (logger->level >= LOG_PRIO(flags))) {
		va_start(args, fmt);
		handler(logger, flags, fmt, args);
		va_end(args);
	}
}
