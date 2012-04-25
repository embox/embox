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

#include <util/macro.h>

#define N_DEBUG_MSG 100  /* Quantity of messages in a queue */
#define MAX_MSG_LENGTH (256+1)  /* Length of a message + '\0' */
#define MAX_MODULE_NAME_LENGTH (50+1)  /* module name */
#define MAX_FUNC_NAME_LENGTH (50+1)  /* function name */

#define MSG_TRUNC_MSG "The message is to long. Trunctuated to 50 symbols\n"
#define MOD_TRUNC_MSG "The module name is to long. Trunctuated to 20 symbols\n"
#define FUNC_TRUNC_MSG "The function name is to long. Trunctuated to 20 symbols\n"

enum log_msg_type{
	LT_INFO,
	LT_WARNING,
	LT_ERROR,
	LT_DEBUG,
	LT_MAX
};

/* message structure in a queue */
typedef struct debug_msg{
	char msg[MAX_MSG_LENGTH];
	char module[MAX_MODULE_NAME_LENGTH];
	char func[MAX_FUNC_NAME_LENGTH];
	unsigned int serial;
	int msg_type;
} debug_msg_t;

extern bool syslog_toggle_intrusive(void);
extern void system_log(char *msg, char *module, char *func, int msg_type);
extern void show_log(unsigned int count, bool *disp_types);

#define LOG_INFO(func_name, msg) system_log(msg, MACRO_STRING(__EMBUILD_MOD__),\
																							func_name, LT_INFO)
#define LOG_ERROR(func_name, msg) system_log(msg, MACRO_STRING(__EMBUILD_MOD__),\
																							 func_name, LT_ERROR)
#define LOG_WARN(func_name, msg) system_log(msg, MACRO_STRING(__EMBUILD_MOD__),\
																							func_name, LT_WARNING)

#endif

