/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#ifndef UTIL_LOG_H_
#define UTIL_LOG_H_

#include <stdarg.h>
#include <stdint.h>
#include <syslog.h>

#include <framework/mod/options.h>
#include <framework/mod/self.h>

#include <config/embox/util/log.h>

#define __LOG_LEVEL OPTION_MODULE_GET(embox__util__log, STRING, max_log_level)

#if __LOG_LEVEL >= LOG_EMERG
#define __log_emerg(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_EMERG, fmt, ##__VA_ARGS__)
#else
#define __log_emerg(fmt, ...)
#endif

#if __LOG_LEVEL >= LOG_ALERT
#define __log_alert(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_ALERT, fmt, ##__VA_ARGS__)
#else
#define __log_alert(fmt, ...)
#endif

#if __LOG_LEVEL >= LOG_CRIT
#define __log_crit(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_CRIT, fmt, ##__VA_ARGS__)
#else
#define __log_crit(fmt, ...)
#endif

#if __LOG_LEVEL >= LOG_ERR
#define __log_error(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_ERR, fmt, ##__VA_ARGS__)
#else
#define __log_error(fmt, ...)
#endif

#if __LOG_LEVEL >= LOG_WARNING
#define __log_warning(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_WARNING, fmt, ##__VA_ARGS__)
#else
#define __log_warning(fmt, ...)
#endif

#if __LOG_LEVEL >= LOG_NOTICE
#define __log_notice(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_NOTICE, fmt, ##__VA_ARGS__)
#else
#define __log_notice(fmt, ...)
#endif

#if __LOG_LEVEL >= LOG_INFO
#define __log_info(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_INFO, fmt, ##__VA_ARGS__)
#else
#define __log_info(fmt, ...)
#endif

#if __LOG_LEVEL >= LOG_DEBUG
#define __log_debug(fmt, ...) \
	__log_send_msg(&mod_logger, LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define __log_debug(fmt, ...)
#endif

#define log_emerg(fmt, ...)   __log_emerg(fmt, ##__VA_ARGS__)
#define log_alert(fmt, ...)   __log_alert(fmt, ##__VA_ARGS__)
#define log_crit(fmt, ...)    __log_crit(fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   __log_error(fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) __log_warning(fmt, ##__VA_ARGS__)
#define log_notice(fmt, ...)  __log_notice(fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    __log_info(fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)   __log_debug(fmt, ##__VA_ARGS__)

#define log_send_msg(prio, fmt, ...) \
	__log_send_msg(&mod_logger, prio, fmt, ##__VA_ARGS__)

typedef uint8_t log_prio_t;
typedef void (*log_handler_t)(struct logger *logger, log_prio_t prio,
    const char *fmt, va_list args);

/**
 * Logging params
 */
struct logging {
	log_prio_t level; /**< Filtering log level */
};

/**
 * Logger structure binding a specific module and logging params
 */
struct logger {
	const struct mod *mod;
	struct logging logging;
};

/**
 * Declares logger of the module from which macro is called.
 */
extern struct logger mod_logger __attribute__((weak));

extern const char *log_prio_prefix(log_prio_t prio);
extern void log_set_handler(log_handler_t handler);

extern void __log_send_msg(struct logger *logger, log_prio_t prio,
    const char *fmt, ...);

#endif /* UTIL_LOG_H_ */
