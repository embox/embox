/**
 * @file
 * @brief debug_msg.c header file
 *
 * @date 13.03.12
 * @author Timur Abdukadyrov
 *
 */


#ifndef UTIL_DEBUG_MSG_H_
#define UTIL_DEBUG_MSG_H_

#include <stdbool.h>

enum log_msg_type{
	LT_INFO,
	LT_WARNING,
	LT_ERROR,
	LT_DEBUG,
	LT_MAX
};

/**
 * @brief TODO
 *
 * @param types
 *
 * @return
 */
extern bool syslog_toggle_intrusive(bool *types);

/**
 * @brief TODO
 *
 * @param msg
 * @param module
 * @param func
 * @param msg_type
 */
extern void system_log(const char *msg, char *module, char *func, int msg_type);

/**
 * @brief logging function with format capability, msg is format, substitutes are variadic
 *
 * @param msg
 * @param module
 * @param func
 * @param msg_type
 * @param ...
 */
extern void vsystem_log(const char *msg, char *module, char *func, int msg_type, ...);

/**
 * @brief
 *
 * @param count
 * @param disp_types
 */
extern void show_log(unsigned int count, bool *disp_types);

#define LOG_INFO(func_name, msg) system_log(msg, MACRO_STRING(__EMBUILD_MOD__),\
		func_name, LT_INFO)
#define LOG_ERROR(func_name, msg) system_log(msg, MACRO_STRING(__EMBUILD_MOD__),\
		func_name, LT_ERROR)
#define LOG_WARN(func_name, msg) system_log(msg, MACRO_STRING(__EMBUILD_MOD__),\
		func_name, LT_WARNING)
#define LOG_DEBUG(func_name, msg) system_log(msg, MACRO_STRING(__EMBUILD_MOD__),\
		func_name, LT_DEBUG)

#endif /* UTIL_DEBUG_MSG_H_ */
