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
#include <kernel/task.h>

/* The condition variable operates upon by any thread that has access to the
 * memory where the condition variable is allocated */
#define PROCESS_SHARED  0
/* The condition variable available by threads within the same task as the thread
 * that initialized the conditional variable. If thread tries to use the permitted
 * conditional variable then a function will return EACCES */
#define PROCESS_PRIVATE 1

struct condattr {
	int pshared;
};

struct cond {
	struct wait_queue wq;
	struct condattr attr;
	struct task* task; //task where cond was created
};

typedef struct cond cond_t;

#define COND_INIT_STATIC \
		{ \
			{ /*wait_queue init */ \
				{/* dlist_init*/ NULL, NULL, NULL,}, \
				/* flags */0, \
				}, \
			{ /*condattr init */ \
				/* pshared */ PROCESS_PRIVATE, \
				}, \
			/* task */ NULL \
		}

__BEGIN_DECLS

extern void cond_init(cond_t *c, const struct condattr *attr);
extern int cond_wait(cond_t *c, struct mutex *m);
extern int cond_signal(cond_t *c);
extern int cond_broadcast(cond_t *c);
extern void cond_destroy(cond_t *c);

extern void condattr_init(struct condattr *attr);
extern void condattr_destroy(struct condattr *attr);
extern void condattr_getpshared(const struct condattr *attr, int *pshared);
extern void condattr_setpshared(struct condattr *attr, int pshared);

__END_DECLS

#endif /* KERNEL_THREAD_SYNC_COND_H_ */
