/**
 * @file
 * @brief Defines condition variable structure and methods associated with it.
 *
 * @date 03.09.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SYNC_COND_H_
#define KERNEL_THREAD_SYNC_COND_H_

#include <kernel/thread/sync/mutex.h>
#include <kernel/sched/wait_queue.h>

struct cond {
	struct wait_queue wq;
};

typedef struct cond cond_t;

__BEGIN_DECLS

extern void cond_init(cond_t *c);
extern void cond_wait(cond_t *c, struct mutex *m);
extern void cond_signal(cond_t *c);
extern void cond_broadcast(cond_t *c);
extern void cond_destroy(cond_t *c);

__END_DECLS

#endif /* KERNEL_THREAD_SYNC_COND_H_ */
