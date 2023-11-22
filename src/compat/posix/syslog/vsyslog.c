/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <stdarg.h>
#include <syslog.h>

#include <util/log.h>

void vsyslog(int priority, const char *message, va_list args) {
	log_handler_t handler;

	if ((handler = log_get_handler())) {
		handler(NULL, LOG_MSG | LOG_PRIO(priority), message, args);
	}
}
