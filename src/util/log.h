/**
 * @file
 * @brief Describes implicit logger for modules. To use implicit logger
 * add log_level option in a mybuild module description of the specific module.
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#ifndef UTIL_LOG_H_
#define UTIL_LOG_H_

#include <framework/mod/options.h>
#include <kernel/printk.h>

#include <config/embox/util/log.h>

#define __LOG_LEVEL_DEFAULT \
	OPTION_MODULE_GET(embox__util__log, NUMBER, log_level_default)
#define __LOG_LEVEL_MAX \
	OPTION_MODULE_GET(embox__util__log, NUMBER, log_level_max)

#if OPTION_DEFINED(NUMBER, log_level)
#define __LOG_LEVEL_CURRENT OPTION_GET(NUMBER, log_level)
#else
#define __LOG_LEVEL_CURRENT __LOG_LEVEL_DEFAULT
#endif

#if __LOG_LEVEL_CURRENT > __LOG_LEVEL_MAX
#undef __LOG_LEVEL_CURRENT
#define __LOG_LEVEL_CURRENT __LOG_LEVEL_MAX
#endif

/**
 * Logging level, decreasing by fatality.
 */
#define LOG_NONE    0
#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4

/**
 * Logging level of current module.
 */
#define LOG_LEVEL   __LOG_LEVEL_CURRENT

/**
 * Logs a raw message in a way specified by the module logger.
 *
 * @param level Message logging level, see #log_level
 * @param fmt   printf-like format of the message
 */
#define log_raw(level, fmt, ...)        \
	if (__LOG_LEVEL_CURRENT >= level) { \
		printk(fmt, ##__VA_ARGS__);     \
	}

/**
 * Logs a formatted message in a way specified by the module logger. A
 * resulting message has a following format:
 * "level: function: message"
 *
 * @param level Message logging level
 * @param fmt   printf-like format of the message
 */
#define log_logp(level, fmt, ...)              \
	if (__LOG_LEVEL_CURRENT >= level) {        \
		__log_logp(level, fmt, ##__VA_ARGS__); \
	}

#define __log_logp(level, fmt, ...) \
	printk("%s: %s: " fmt "\n", log_levels[level - 1], __func__, ##__VA_ARGS__)

#if __LOG_LEVEL_CURRENT >= LOG_ERROR
#define log_error(fmt, ...) __log_logp(LOG_ERROR, fmt, ##__VA_ARGS__)
#else
#define log_error(fmt, ...)
#endif

#if __LOG_LEVEL_CURRENT >= LOG_WARNING
#define log_warning(fmt, ...) __log_logp(LOG_WARNING, fmt, ##__VA_ARGS__)
#else
#define log_warning(fmt, ...)
#endif

#if __LOG_LEVEL_CURRENT >= LOG_INFO
#define log_info(fmt, ...) __log_logp(LOG_INFO, fmt, ##__VA_ARGS__)

#define log_boot(fmt, ...) printk("\t\t\t" fmt, ##__VA_ARGS__)

#define log_boot_start()   printk("\n")

#define log_boot_stop()    printk("\t\t")
#else
#define log_info(fmt, ...)

#define log_boot(fmt, ...)

#define log_boot_start()

#define log_boot_stop()
#endif

#if __LOG_LEVEL_CURRENT >= LOG_DEBUG
#define log_debug(fmt, ...) __log_logp(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif

/**
 * Array mapping a log_level on its text view.
 * log_level - 1 -> "level"
 */
extern const char *log_levels[];

#endif /* UTIL_LOG_H_ */
