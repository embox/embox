/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <assert.h>
#include <stdarg.h>
#include <syslog.h>

#include "log.h"

static log_handler_t log_handler;

const char *log_prio_prefix(log_prio_t prio) {
	static const char *log_priorities[LOG_DEBUG] = {
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

void __log_send_msg(struct logger *logger, log_prio_t prio, const char *fmt,
    ...) {
	va_list args;

	if (log_handler) {
		va_start(args, fmt);
		log_handler(logger, prio, fmt, args);
		va_end(args);
	}
}
