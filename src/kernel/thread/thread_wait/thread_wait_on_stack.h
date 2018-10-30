/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#ifndef THREAD_WAIT_ON_STACK_H_
#define THREAD_WAIT_ON_STACK_H_

#include <util/dlist.h>

#include <kernel/time/timer.h>

struct thread_wait {
	struct dlist_head thread_waitq_list;
};

#define SCHED_WAIT_TIMER_DEF(_tmr) \
	struct sys_timer _tmr;

#define SCHED_WAIT_TIMER_INIT(_tmr, flags, msec, handler, param) \
	timer_init_start_msec(&_tmr, flags, msec, handler, param)

#define SCHED_WAIT_TIMER_ADD(_tmr) \
	thread_wait_add(&(thread_self()->thread_wait_list), &_tmr)

#define SCHED_WAIT_TIMER_CLOSE(_tmr) \
	timer_close(&_tmr);\
	thread_wait_del(&(thread_self()->thread_wait_list), &_tmr)

#endif /* THREAD_WAIT_ON_STACK_H_ */


