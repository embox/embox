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
#include <sys/cdefs.h>
#include <syslog.h>

#include <framework/mod/self.h>

#include <module/embox/util/log.h>

#define LOG_RAW 0U
#define LOG_BEG (1U << 8)
#define LOG_END (1U << 9)
#define LOG_MSG (LOG_BEG | LOG_END)

#if __MAX_LOG_LEVEL >= LOG_EMERG
#define __LOG_EMERG(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_EMERG(flags, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_ALERT
#define __LOG_ALERT(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_ALERT(flags, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_CRIT
#define __LOG_CRIT(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_CRIT(flags, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_ERR
#define __LOG_ERR(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_ERR(flags, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_WARNING
#define __LOG_WARNING(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_WARNING(flags, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_NOTICE
#define __LOG_NOTICE(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_NOTICE(flags, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_INFO
#define __LOG_INFO(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_INFO(flags, fmt, ...)
#endif

#if __MAX_LOG_LEVEL >= LOG_DEBUG
#define __LOG_DEBUG(flags, fmt, ...) __log_handle(flags, fmt, ##__VA_ARGS__)
#else
#define __LOG_DEBUG(flags, fmt, ...)
#endif

#define log_raw(prio, fmt, ...) __##prio(LOG_RAW | prio, fmt, ##__VA_ARGS__)
#define log_beg(prio, fmt, ...) __##prio(LOG_BEG | prio, fmt, ##__VA_ARGS__)
#define log_end(prio, fmt, ...) __##prio(LOG_END | prio, fmt, ##__VA_ARGS__)
#define log_msg(prio, fmt, ...) __##prio(LOG_MSG | prio, fmt, ##__VA_ARGS__)

#define log_emerg(fmt, ...)     log_msg(LOG_EMERG, fmt, ##__VA_ARGS__)
#define log_alert(fmt, ...)     log_msg(LOG_ALERT, fmt, ##__VA_ARGS__)
#define log_crit(fmt, ...)      log_msg(LOG_CRIT, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)     log_msg(LOG_ERR, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...)   log_msg(LOG_WARNING, fmt, ##__VA_ARGS__)
#define log_notice(fmt, ...)    log_msg(LOG_NOTICE, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)      log_msg(LOG_INFO, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)     log_msg(LOG_DEBUG, fmt, ##__VA_ARGS__)

#define log_level_self()        __LOG_LEVEL_SELF

typedef uint8_t log_prio_t;

#define LOG_PRIO(flags) (log_prio_t)(flags)

typedef void (*log_handler_t)(struct logger *logger, uint16_t flags,
    const char *fmt, va_list args);

struct logger {
	const struct mod *mod;
	log_prio_t level;
};

__BEGIN_DECLS

extern const char *log_prio2str(log_prio_t prio);
extern void log_set_handler(log_handler_t handler);
extern log_handler_t log_get_handler(void);

/* Declares logger of the module from which macro is called. */
extern struct logger mod_logger __attribute__((weak));

__END_DECLS

#endif /* UTIL_LOG_H_ */
