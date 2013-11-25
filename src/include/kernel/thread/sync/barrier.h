/**
 * @file
 * @brief Defines barrier structure and methods associated with it.
 *
 * @date 03.09.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SYNC_BARRIER_H_
#define KERNEL_THREAD_SYNC_BARRIER_H_

#include <kernel/sched/waitq.h>

struct barrier {
	struct waitq wq;
	int current_count;
	int count;
};

typedef struct barrier barrier_t;

extern void barrier_init(barrier_t *b, int count);
extern void barrier_wait(barrier_t *b);

#endif /* KERNEL_THREAD_SYNC_BARRIER_H_ */
