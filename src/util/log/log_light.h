/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#ifndef UTIL_LOG_LIGHT_H_
#define UTIL_LOG_LIGHT_H_

#include <syslog.h>

#include <framework/mod/options.h>

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

#endif /* UTIL_LOG_LIGHT_H_ */
