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

enum runnable_result {
	RUNNABLE_EXIT,
	RUNNABLE_REPEAT
};

struct runnable {
	/**
	 * Thread-specific function, puts current thread in runq
	 */
	enum runnable_result                (*prepare)(struct runnable *prev,
			struct runnable *n, struct runq *rq);

	void               *(*run)(void *); /**< Start routine */
	void               *run_arg;        /**< Argument to be passed to run */

	struct sched_attr  sched_attr;      /**< Scheduler-private data pointer */

	unsigned int       active;       /**< Running on a CPU. TODO SMP-only. */
	unsigned int       ready;        /**< Managed by the scheduler. */
	unsigned int       waiting;      /**< Waiting for an event. */

	spinlock_t         lock;         /**< Protects wait state and others. */
};

#endif /* _KERNEL_RUNNABLE_H_ */
