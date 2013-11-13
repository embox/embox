/*
	Describes lightweight thread structure.
*/

#ifndef _KERNEL_LIGHTTHREAD_H_
#define _KERNEL_LIGHTTHREAD_H_

struct lthread {
	void               *(*run)(void *); /**< Start routine. */
	void               *run_arg;        /**< Argument to pass to start routine. */

	int                policy;       /**< Scheduling policy pointer*/
	struct sched_attr  sched_attr;   /**< Scheduler-private data pointer */
}

extern void lthread_init(struct lthread *lt, void *(*run)(void *), void *arg);
extern struct lthread * lthread_create(void *(*run)(void *), void *arg);

#endif /* _KERNEL_LIGHTTHREAD_H_ */
