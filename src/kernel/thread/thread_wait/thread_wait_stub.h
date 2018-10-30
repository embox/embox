/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#ifndef THREAD_WAIT_STUB_H_
#define THREAD_WAIT_STUB_H_

#include <kernel/time/timer.h>

struct thread_wait {
};

#define SCHED_WAIT_TIMER_DEF(_tmr) \
	struct sys_timer *_tmr;

#define SCHED_WAIT_TIMER_INIT(_tmr, flags, msec, handler, param) \
	timer_set(&_tmr, flags, msec, handler, param)

#define SCHED_WAIT_TIMER_ADD(_tmr)

#define SCHED_WAIT_TIMER_CLOSE(_tmr) \
	timer_close(_tmr)

#endif /* THREAD_WAIT_STUB_H_ */
