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
	void                *(*prepare)(struct runnable *prev, struct runnable *n, struct runq *rq);

	void               *(*run)(void *); /**< Start routine */
	void               *run_arg;        /**< Argument to be passed to run */

	struct sched_attr  sched_attr;      /**< Scheduler-private data pointer */
};

#endif /* _KERNEL_RUNNABLE_H_ */
