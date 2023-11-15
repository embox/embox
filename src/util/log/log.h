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

#include <framework/mod/self.h>

#include <module/embox/util/log.h>

#define LOG_PREFIX      (1U << 8)
#define LOG_POSTFIX     (1U << 9)
#define LOG_PRIO(flags) (log_prio_t)(flags)

#define __LOG_RAW       0U
#define __LOG_START     LOG_PREFIX
#define __LOG_STOP      LOG_POSTFIX
#define __LOG_MSG       (LOG_PREFIX | LOG_POSTFIX)

#if __MAX_LOG_LEVEL >= LOG_EMERG
#define __log_emerg(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_EMERG | type, fmt, ##__VA_ARGS__)
#else
#define __log_emerg(type, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_ALERT
#define __log_alert(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_ALERT | type, fmt, ##__VA_ARGS__)
#else
#define __log_alert(type, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_CRIT
#define __log_crit(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_CRIT | type, fmt, ##__VA_ARGS__)
#else
#define __log_crit(type, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_ERR
#define __log_error(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_ERR | type, fmt, ##__VA_ARGS__)
#else
#define __log_error(type, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_WARNING
#define __log_warning(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_WARNING | type, fmt, ##__VA_ARGS__)
#else
#define __log_warning(type, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_NOTICE
#define __log_notice(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_NOTICE | type, fmt, ##__VA_ARGS__)
#else
#define __log_notice(type, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_INFO
#define __log_info(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_INFO | type, fmt, ##__VA_ARGS__)
#else
#define __log_info(type, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_DEBUG
#define __log_debug(type, fmt, ...) \
	__log_handle(&mod_logger, LOG_DEBUG | type, fmt, ##__VA_ARGS__)
#else
#define __log_debug(type, fmt, ...)
#endif

#define log_raw(fn, ...)      __##fn(__LOG_RAW, fmt, ##__VA_ARGS__)
#define log_start(fn, ...)    __##fn(__LOG_START, fmt, ##__VA_ARGS__)
#define log_stop(fn, ...)     __##fn(__LOG_STOP, fmt, ##__VA_ARGS__)

#define log_emerg(fmt, ...)   __log_emerg(__LOG_MSG, fmt, ##__VA_ARGS__)
#define log_alert(fmt, ...)   __log_alert(__LOG_MSG, fmt, ##__VA_ARGS__)
#define log_crit(fmt, ...)    __log_crit(__LOG_MSG, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   __log_error(__LOG_MSG, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) __log_warning(__LOG_MSG, fmt, ##__VA_ARGS__)
#define log_notice(fmt, ...)  __log_notice(__LOG_MSG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    __log_info(__LOG_MSG, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)   __log_debug(__LOG_MSG, fmt, ##__VA_ARGS__)

typedef uint8_t log_prio_t;

typedef void (*log_handler_t)(struct logger *logger, unsigned flags,
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

extern const char *log_get_prefix(log_prio_t prio);
extern void log_set_handler(log_handler_t handler);

extern void __log_handle(struct logger *logger, unsigned flags, const char *fmt,
    ...);

#endif /* UTIL_LOG_H_ */
