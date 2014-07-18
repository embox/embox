/**
 * @file
 * @brief Interface for thread entities, used by scheduler
 *
 * @date 21.11.2013
 * @author Andrey Kokorev
 */

#ifndef _KERNEL_RUNNABLE_H_
#define _KERNEL_RUNNABLE_H_

#include <kernel/sched/sched_strategy.h>

struct runnable {
	/**
	 * Thread-specific function, puts current thread in runq
	 */
	void                *(*prepare)(struct thread *prev, struct runnable *n);

	void               *(*run)(void *); /**< Start routine */
	void               *run_arg;        /**< Argument to be passed to run */

	struct sched_attr  sched_attr;      /**< Scheduler-private data pointer */
};

/**
 * Theese are assigned to runnable->shed_prepare and runnable->run in core.c
 *
 * @param prev
 *   next thread
 * @param next
 *   next thread runnable
 */
extern void sched_prepare_thread(struct thread *prev, struct runnable *next);

#endif /* _KERNEL_RUNNABLE_H_ */
