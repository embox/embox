/**
 * @file
 * @brief
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#ifndef UTIL_LOG_H_
#define UTIL_LOG_H_

#include <framework/mod/self.h>
#include <util/logging.h>

struct logger {
	const struct mod *mod;
	struct logging logging;
};

#define LOG_DECLARE_LOGGER() \
	extern struct logger mod_logger __attribute__ ((weak))

#define log_logp(level, fmt, ...) \
	if (!&mod_logger) \
		logging_raw(&mod_logger.logging, level, "%s: %s: " fmt "\n", \
					log_levels[level-1], __func__, ## __VA_ARGS__)

#define log_raw(level, fmt, ...) \
	if (!&mod_logger) \
		logging_raw(&mod_logger.logging, level, fmt, ## __VA_ARGS__)

#define log_debug(fmt, ...) \
	log_logp(LOG_DEBUG, fmt, ## __VA_ARGS__)
#define log_info(fmt, ...) \
	log_logp(LOG_INFO, fmt, ## __VA_ARGS__)
#define log_warning(fmt, ...) \
	log_logp(LOG_WARNING, fmt, ## __VA_ARGS__)
#define log_error(fmt, ...) \
	log_logp(LOG_ERROR, fmt, ## __VA_ARGS__)

#endif /* UTIL_LOG_H_ */
