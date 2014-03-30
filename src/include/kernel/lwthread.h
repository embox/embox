/*
	Describes lightweight thread structure.
*/

#ifndef _KERNEL_LWTHREAD_H_
#define _KERNEL_LWTHREAD_H_
#include <kernel/runnable/runnable.h>

struct lwthread {
	struct runnable runnable;
};

/*
 * Called by sched in __schedule
 */
extern void lwthread_trampoline(struct runnable *r);

/*
 * Allocate new lwthread
 */
extern struct lwthread * lwthread_create(void *(*run)(void *), void *arg);
/*
 * Adds lwthread to runq
 */
extern void lwthread_launch(struct lwthread *lwt);
extern void lwthread_free(struct lwthread *lwt);

#endif /* _KERNEL_LWTHREAD_H_ */
