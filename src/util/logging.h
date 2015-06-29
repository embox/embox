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
enum log_level{
	LOG_NONE=0,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG
};

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
