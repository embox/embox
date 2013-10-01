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

/* The condition variable operates upon by any thread that has access to the
 * memory where the condition variable is allocated, even if the condition variable
 * is allocated in memory that is shared by multiple process */
#define PROCESS_SHARED 	0;
/*  The condition variable will only be operated upon by threads created within
 * the same process as the thread that initialized the condition variable */
#define PROCESS_PRIVATE 1;

struct condattr {
	int pshared;
};

struct cond {
	struct wait_queue wq;
	struct condattr attr;
};

typedef struct cond cond_t;

__BEGIN_DECLS

extern void cond_init(cond_t *c, const struct condattr *attr);
extern void cond_wait(cond_t *c, struct mutex *m);
extern void cond_signal(cond_t *c);
extern void cond_broadcast(cond_t *c);
extern void cond_destroy(cond_t *c);

extern void condattr_init(struct condattr *attr);
extern void condattr_destroy(struct condattr *attr);
extern void condattr_getpshared(const struct condattr *attr, int *pshared);
extern void condattr_setpshared(struct condattr *attr, int pshared);

__END_DECLS

#endif /* KERNEL_THREAD_SYNC_COND_H_ */
