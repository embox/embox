/*
	Describes lightweight thread structure.
*/

#ifndef _KERNEL_LTHREAD_H_
#define _KERNEL_LTHREAD_H_

#include <kernel/runnable/runnable.h>

struct lthread {
	struct runnable runnable;
};

/*
 * Called by sched in __schedule
 */
extern void lthread_trampoline(struct runnable *r);

/*
 * Allocate new lthread
 */
extern struct lthread * lthread_create(void *(*run)(void *), void *arg);
/*
 * Adds lthread to runq
 */
extern void lthread_launch(struct lthread *lwt);

#endif /* _KERNEL_LTHREAD_H_ */
