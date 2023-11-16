/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <stdarg.h>
#include <syslog.h>

#include "log.h"

static log_handler_t log_handler;

const char *log_get_prefix(log_prio_t prio) {
	static const char *const log_priorities[LOG_DEBUG] = {
	    "emerg:   ",
	    "alert:   ",
	    "crit:    ",
	    "error:   ",
	    "warning: ",
	    "notice:  ",
	    "info:    ",
	    "debug:   ",
	};

	return log_priorities[prio - 1];
}

void log_set_handler(log_handler_t handler) {
	log_handler = handler;
}

void __log_handle(struct logger *logger, unsigned flags, const char *fmt, ...) {
	va_list args;

	/**
	 * logger == NULL if log_level is set, but embox.framework.embuild_light
	 * module is used
	 */
	if ((!logger || (logger->logging.level >= LOG_PRIO(flags)))
	    && log_handler) {
		va_start(args, fmt);
		log_handler(logger, flags, fmt, args);
		va_end(args);
	}
}
