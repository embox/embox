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
static char pool[N_MSG*N_DEBUG_MSG];

static unsigned int N_messages = 0;
/* static unsigned int tail = 0; */

void debug_printf(char *msg){

	if(strlen(msg)>N_MSG){
		if(strlen(TRUNC_MSG)<=N_MSG)/* just in case someone someday alters TRUNC_MSG*/
			debug_printf(TRUNC_MSG);
	}

	strcpy(pool + N_messages*N_MSG, msg);

	/* cyclic incrementation */
	N_messages += 1;
	if(N_messages == N_DEBUG_MSG)
		N_messages = 0;
}


void output_debug_messages(unsigned int count){
	int n, current = N_messages + (N_DEBUG_MSG-count) + 1;

	for(n=0; n<count; n++){
		printk("%d: %s", n, pool+current*N_MSG);
		current++;
		if(current>=N_DEBUG_MSG)
			current = 0;
	}
}
