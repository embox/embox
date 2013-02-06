/*
 * sched_posix.c
 *
 *  Created on: 5 feb. 2013
 *      Author: fsulima
 */

#include <sched.h>

#include <kernel/thread/sched.h>

int sched_yield(void) {
	sched_post_switch();
	return 0;
}
