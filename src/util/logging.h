/**
 * @file
 * @brief
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#ifndef UTIL_LOGGING_H_
#define UTIL_LOGGING_H_

/**
 * Logging level, decreasing by fatality.
 */
#define LOG_NONE    0
#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4

/**
 * Logging params
 */
struct logging {
	int level; /**< Filtering log level */
};


/**
 * Array mapping a log_level on its text view.
 * log_level - 1 -> "level"
 */
extern char *log_levels[];

/**
 * Logs message in a way specified by @p logging.
 *
 * @param logging Logging params
 * @param level   Level of the message
 * @param fmt     printf-like format of the message
 */
extern void logging_raw(struct logging *logging, int level,
	const char* fmt, ...);

#endif /* UTIL_LOGGING_H_ */
