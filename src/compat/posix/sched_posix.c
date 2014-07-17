/*
 * sched_posix.c
 *
 *  Created on: 5 feb. 2013
 *      Author: fsulima
 */

#include <sched.h>

#include <kernel/sched.h>

int sched_yield(void) {
	sched_post_switch();
	return 0;
}

int sched_get_priority_min(int policy) {
	return SCHED_PRIORITY_MIN;
}

int sched_get_priority_max(int policy) {
	return SCHED_PRIORITY_MAX;
}

int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
	return 0;
}
