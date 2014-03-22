/**
	Interface for thread entities, used by scheduler
*/
#ifndef _KERNEL_RUNNABLE_H_
#define _KERNEL_RUNNABLE_H_


#include <kernel/sched/sched_strategy.h>

struct runnable {
	/**
		Thread-specific function, puts current thread in runq
	*/
	void                *(*prepare)(struct thread *prev, struct runnable *n);

	/** Called by sched when replanned.
	*/
	void               *(*run)(void *);
	void			   *run_arg;     /* Argument to be passed to run function */

	struct sched_attr  sched_attr;   /**< Scheduler-private data pointer */
};

extern struct runnable *runnable_get_current(void);

/* Theese are assigned to runnable->shed_prepare and runnable->run in core.c */
extern void sched_prepare_thread(struct thread *prev, struct runnable *n);
/*extern void sched_execute_runnable(struct runnable *p, struct runnable *n, struct runq *rq);*/


#endif /* _KERNEL_RUNNABLE_H_ */
