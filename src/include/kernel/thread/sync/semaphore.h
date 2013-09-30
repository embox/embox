/**
 * @file
 * @brief Defines semaphore structure and methods associated with it.
 *
 * @date 02.09.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SYNC_SEMAPHORE_H_
#define KERNEL_THREAD_SYNC_SEMAPHORE_H_

#include <kernel/sched/wait_queue.h>

struct sem {
	struct wait_queue wq;
	int value;
	int max_value;
};

extern void semaphore_init(struct sem *s, int val);
extern void semaphore_enter(struct sem *s);
extern void semaphore_leave(struct sem *s);
extern int semaphore_tryenter(struct sem *s);

extern int semaphore_getvalue(struct sem *restrict s, int *restrict sval);

#endif /* KERNEL_THREAD_SYNC_SEMAPHORE_H_ */
