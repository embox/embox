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

#include <sys/cdefs.h>

#include <framework/mod/self.h>

/**
 * Logging level, decreasing by fatality.
 */
#define LOG_NONE    0
#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4

/**
 * Logs a formatted message in a way specified by the module logger. A
 * resulting message has a following format:
 * "level: function: message"
 *
 * @param level Message logging level
 * @param fmt   printf-like format of the message
 */
#define log_logp(level, fmt, ...)                                \
	if (&mod_logger)                                             \
	logging_raw(&mod_logger.logging, level, "%s: %s: " fmt "\n", \
	    log_levels[level - 1], __func__, ##__VA_ARGS__)

/**
 * Logs a raw message in a way specified by the module logger.
 *
 * @param level Message logging level, see #log_level
 * @param fmt   printf-like format of the message
 */
#define log_raw(level, fmt, ...) \
	if (&mod_logger)             \
	logging_raw(&mod_logger.logging, level, fmt, ##__VA_ARGS__)

#define log_debug(fmt, ...)   log_logp(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    log_logp(LOG_INFO, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) log_logp(LOG_WARNING, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   log_logp(LOG_ERROR, fmt, ##__VA_ARGS__)

#define log_boot(fmt, ...) \
	if (&mod_logger)       \
	logging_raw(&mod_logger.logging, LOG_INFO, "\t\t\t" fmt "", ##__VA_ARGS__)

#define log_boot_start() \
	if (&mod_logger)     \
	logging_raw(&mod_logger.logging, LOG_INFO, "\n")

#define log_boot_stop() \
	if (&mod_logger)    \
	logging_raw(&mod_logger.logging, LOG_INFO, "\t\t")

/**
 * Logging params
 */
struct logging {
	int level; /**< Filtering log level */
};

/**
 * Logger structure binding a specific module and logging params.
 */
struct logger {
	const struct mod *mod;
	struct logging logging;
};

/**
 * Declares logger of the module from which macro is called.
 */
extern struct logger mod_logger __attribute__((weak));

/**
 * Array mapping a log_level on its text view.
 * log_level - 1 -> "level"
 */
extern char *log_levels[];

__BEGIN_DECLS

/**
 * Logs message in a way specified by @p logging.
 *
 * @param logging Logging params
 * @param level   Level of the message
 * @param fmt     printf-like format of the message
 */
extern void logging_raw(struct logging *logging, int level, const char *fmt,
    ...);

__END_DECLS

#endif /* UTIL_LOG_H_ */
