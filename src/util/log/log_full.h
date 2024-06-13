/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#ifndef UTIL_LOG_FULL_H_
#define UTIL_LOG_FULL_H_

#include <stdint.h>
#include <sys/cdefs.h>
#include <syslog.h>

#include <framework/mod/options.h>

#include "log.h"

#if OPTION_DEFINED(STRING, log_level)
#define __MAX_LOG_LEVEL \
	OPTION_MODULE_GET(embox__util__log, STRING, max_log_level)
#else
#define __MAX_LOG_LEVEL LOG_NONE
#endif

#define __LOG_LEVEL_SELF (mod_logger.level)

#define __log_handle(flags, fmt, ...) \
	log_full_handle(&mod_logger, flags, fmt, ##__VA_ARGS__)

__BEGIN_DECLS

extern void log_full_handle(struct logger *logger, uint16_t flags,
    const char *fmt, ...);

__END_DECLS

#endif /* UTIL_LOG_FULL_H_ */
