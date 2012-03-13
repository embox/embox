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
#define N_MSG (50+1)  /* Length of a message + NULL*/

#define TRUNC_MSG "The string is to long. Should be < 50 symbols\n"

/* message structure in a queue */
typedef struct debug_msg{
	char msg[N_MSG];
} debug_msg_t;

void debug_printf(char *msg);
void output_debug_messages(unsigned int count);

#endif
