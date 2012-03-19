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

#define N_DEBUG_MSG 100  /* Quantity of messages in a queue */
#define MAX_MSG_LENGTH (50+1)  /* Length of a message + '\0' */
#define MAX_MODULE_NAME_LENGTH (20+1)  /* module name */
#define MAX_FUNC_NAME_LENGTH (20+1)  /* function name */

#define MODULE_NAME "utils"
#define MSG_TRUNC_MSG "The message is to long. Trunctuated to 50 symbols\n"
#define MOD_TRUNC_MSG "The module name is to long. Trunctuated to 20 symbols\n"
#define FUNC_TRUNC_MSG "The function name is to long. Trunctuated to 20 symbols\n"

/* message structure in a queue */
typedef struct debug_msg{
	char msg[MAX_MSG_LENGTH];
	char module[MAX_MODULE_NAME_LENGTH];
	char func[MAX_FUNC_NAME_LENGTH];
	unsigned int serial;
} debug_msg_t;

void debug_printf(char *msg, char *module, char *func);
void output_debug_messages(unsigned int count);

#endif
