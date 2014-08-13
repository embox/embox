/*
 * lthread_sched_wait.h
 *
 *  Created on: Aug 12, 2014
 *      Author: vita
 */

#ifndef LTHREAD_SCHED_WAIT_H_
#define LTHREAD_SCHED_WAIT_H_

#include <kernel/time/timer.h>

#define SCHED_WAIT_FINISHED 0
#define SCHED_WAIT_STARTED 1

struct sched_wait_info {
	struct sys_timer tmr;
	clock_t remain;
	clock_t cur_time;
	clock_t prev_time;
	int status;
};

extern void sched_wait_prepare_lthread(void);
extern void sched_wait_cleanup_lthread(void);
extern int sched_wait_timeout_lthread(clock_t timeout);
extern int sched_wait_lthread(void);
extern void sched_wait_info_init(struct sched_wait_info *info);

#endif /* LTHREAD_SCHED_WAIT_H_ */
