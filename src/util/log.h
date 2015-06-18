/**
 * @file
 * @brief
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#ifndef UTIL_LOG_H_
#define UTIL_LOG_H_

enum log_level{
	LOG_NONE=0,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG
};

struct logger {
	char module[50];
	int level;
};

extern char *log_levels[];

#define LOG_INIT(logger_name, module, level) \
	static struct logger (logger_name) = {(module), (level)}

extern void log_raw(struct logger *logger, int level, const char* fmt, ...);

#define log_logp(logger, level, message, ...) \
	log_raw(logger, level, "%s: %s: " message "\n", log_levels[level-1], __func__, ## __VA_ARGS__)

#define log_debug(l, msg, ...)   log_logp(l, LOG_DEBUG, msg, ## __VA_ARGS__)
#define log_info(l, msg, ...)    log_logp(l, LOG_INFO, msg, ## __VA_ARGS__)
#define log_warning(l, msg, ...) log_logp(l, LOG_WARNING, msg, ## __VA_ARGS__)
#define log_error(l, msg, ...)   log_logp(l, LOG_ERROR, msg, ## __VA_ARGS__)

#endif /* UTIL_LOG_H_ */
