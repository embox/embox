/**
 * @file
 * @brief Defines read-write lock structure and methods associated with it.
 *
 * @date 04.09.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SYNC_RWLOCK_H_
#define KERNEL_THREAD_SYNC_RWLOCK_H_

#include <kernel/sched/waitq.h>

struct rwlock {
	struct waitq wq;
	int status;
	int count;
};

typedef struct rwlock rwlock_t;

extern void rwlock_init(rwlock_t *r);
extern void rwlock_read_up(rwlock_t *r);
extern void rwlock_read_down(rwlock_t *r);
extern void rwlock_write_up(rwlock_t *r);
extern void rwlock_write_down(rwlock_t *r);
extern void rwlock_any_down(rwlock_t *r);


#endif /* KERNEL_THREAD_SYNC_RWLOCK_H_ */
