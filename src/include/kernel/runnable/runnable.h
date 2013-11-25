/**
	Interface for thread entities, used by scheduler
*/
#ifndef _KERNEL_RUNNABLE_H_
#define _KERNEL_RUNNABLE_H_


#include <kernel/sched/sched_strategy.h>

struct runnable {
	/**
	*/
	/*void                *(*sched_thread_); */

	/** Called by sched when replanned.
	*/
	void               *(*run)(struct runnable *p, struct runnable *n, struct runq *rq);
	void			   *run_arg;     /* Argument to be passed to run function */

	int                policy;       /**< Scheduling policy pointer*/
	struct sched_attr  sched_attr;   /**< Scheduler-private data pointer */
};

#endif /* _KERNEL_RUNNABLE_H_ */
