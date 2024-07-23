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

#include <framework/mod/decls.h>
#include <framework/mod/ops.h>

#include <config/embox/util/log.h>

#define __mod_logger_self __MOD_LOGGER(__EMBUILD_MOD__)

#if USE_MOD_LOGGER

#if OPTION_DEFINED(STRING, log_level)
#define __MAX_LOG_LEVEL \
	OPTION_MODULE_GET(embox__util__log, STRING, max_log_level)
#else
#define __MAX_LOG_LEVEL LOG_NONE
#endif

#define __LOG_LEVEL_SELF (__MOD_LOGGER(__EMBUILD_MOD__).level)

#define __log_handle(flags, fmt, ...) \
	__log_handle_mod(&__MOD_LOGGER(__EMBUILD_MOD__), flags, fmt, ##__VA_ARGS__)

#else /* !USE_MOD_LOGGER */

#define __MAX_LOG_LEVEL \
	OPTION_MODULE_GET(embox__util__log, STRING, max_log_level)

#if OPTION_DEFINED(STRING, log_level)
#define __LOG_LEVEL OPTION_GET(STRING, log_level)
#else
#define __LOG_LEVEL LOG_NONE
#endif

#if __MAX_LOG_LEVEL > __LOG_LEVEL
#undef __MAX_LOG_LEVEL
#define __MAX_LOG_LEVEL __LOG_LEVEL
#endif

#define __LOG_LEVEL_SELF __MAX_LOG_LEVEL

#define __log_handle(flags, fmt, ...) \
	__log_handle_static(flags, fmt, ##__VA_ARGS__)

#endif /* USE_MOD_LOGGER */

#define LOG_RAW         0
#define LOG_BEG         (1 << 8)
#define LOG_END         (1 << 9)
#define LOG_MSG         (LOG_BEG | LOG_END)
#define LOG_PRIO(flags) (0xff & (flags))

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

__BEGIN_DECLS

typedef int log_prio_t;

struct mod_logger {
	const char *mod_name;
	log_prio_t level;
};

typedef void (*log_handler_t)(struct mod_logger *logger, int flags,
    const char *fmt, va_list args);

/* Declares logger of the module from which macro is called. */
extern struct mod_logger __MOD_LOGGER(__EMBUILD_MOD__) __attribute__((weak));

extern void __log_handle_static(int flags, const char *fmt, ...);
extern void __log_handle_mod(struct mod_logger *logger, int flags,
    const char *fmt, ...);

extern const char *log_prio2str(log_prio_t prio);
extern void log_set_handler(log_handler_t handler);
extern log_handler_t log_get_handler(void);

__END_DECLS

#endif /* UTIL_LOG_H_ */
