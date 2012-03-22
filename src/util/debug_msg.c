/**
 * @file
 * @brief A temporary substitube for STDERR
 *
 * @date 13.03.12
 * @author Timur Abdukadyrov
 *
 */

#include <stdio.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <util/debug_msg.h>
#include <kernel/prom_printf.h>

/* POOL_DEF(debug_msg_pool, struct debug_msg, N_DEBUG_MSG); */
/* static char pool[N_MSG*N_DEBUG_MSG]; */
static debug_msg_t log[N_DEBUG_MSG];

static unsigned int N_message = 0;
static unsigned int serial = 0;
/* static unsigned int tail = 0; */

void debug_printf(char *msg, char *module, char *func){

	/* truncuate if necessary */
	if(strlen(msg)>MAX_MSG_LENGTH){
		if(strlen(MSG_TRUNC_MSG)<=MAX_MSG_LENGTH)/* just in case*/
			debug_printf(MSG_TRUNC_MSG, MODULE_NAME, "debug_printf");
		strncpy(log[N_message].msg, msg, MAX_MSG_LENGTH);
	}else
		strcpy(log[N_message].msg, msg);
	if(strlen(module)>MAX_MODULE_NAME_LENGTH){
		if(strlen(MOD_TRUNC_MSG)<=MAX_MSG_LENGTH)/* just in case*/
			debug_printf(MOD_TRUNC_MSG, MODULE_NAME, "debug_printf");
		strncpy(log[N_message].module, module, MAX_MODULE_NAME_LENGTH);
	}else
		strcpy(log[N_message].module, module);
	if(strlen(func)>MAX_FUNC_NAME_LENGTH){
		if(strlen(FUNC_TRUNC_MSG)<=MAX_MSG_LENGTH)/* just in case*/
			debug_printf(FUNC_TRUNC_MSG, MODULE_NAME, "debug_printf");
		strncpy(log[N_message].func, func, MAX_FUNC_NAME_LENGTH);
	}else
		strcpy(log[N_message].func, func);
	log[N_message].serial = serial;

	/* cyclic incrementation */
	serial++;
	N_message++;
	if(N_message == N_DEBUG_MSG)
		N_message = 0;
}


void output_debug_messages(unsigned int count){
	int n, final_count, current;

	if(!serial){ /* if there are no messages to display */
		printk("No messages to display\n");
		return;
	}

	final_count = count < serial ? count : serial;
  current = (N_message - final_count)%N_DEBUG_MSG;

	for(n=0; n<final_count; n++){
		printk("#%d\t[%s][function %s]: %s\n", log[current].serial,
					 log[current].module, log[current].func, log[current].msg);
		current++;
		if(current>=N_DEBUG_MSG)
			current = 0;
	}
}
