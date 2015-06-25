/**
 * @file
 * @brief
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#ifndef UTIL_LOGGING_H_
#define UTIL_LOGGING_H_

enum log_level{
	LOG_NONE=0,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG
};

struct logging {
	int level;
};

extern char *log_levels[];

extern void logging_raw(struct logging *logging, int level,
	const char* fmt, ...);

#endif /* UTIL_LOGGING_H_ */
