/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <syslog.h>

#include "log.h"

static log_handler_t __log_handler;

const char *log_prio2str(log_prio_t prio) {
	static const char *const log_priorities[LOG_DEBUG] = {
	    "emerg",
	    "alert",
	    "crit",
	    "error",
	    "warning",
	    "notice",
	    "info",
	    "debug",
	};

	return log_priorities[prio - 1];
}

void log_set_handler(log_handler_t handler) {
	__log_handler = handler;
}

log_handler_t log_get_handler(void) {
	return __log_handler;
}
