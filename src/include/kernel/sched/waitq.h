/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_SCHED_WAITQ_H_
#define KERNEL_SCHED_WAITQ_H_

#include <assert.h>

#include <sys/cdefs.h>
#include <util/dlist.h>
#include <kernel/spinlock.h>

__BEGIN_DECLS

struct thread;

struct waitq {
	struct dlist_head list;
	spinlock_t lock;
};

struct waitq_link {
	struct dlist_head link;
	int result;
};

#define WAITQ_INIT(wq)  { \
	.list = DLIST_INIT(wq.list), \
	.lock = SPIN_UNLOCKED,       \
}

static inline void waitq_init(struct waitq *wq) {
	dlist_init(&wq->list);
	wq->lock = SPIN_UNLOCKED;
}

static inline void waitq_link_init(struct waitq_link *wql) {
	dlist_init(&wql->link);
}

extern void __waitq_enqueue(struct waitq *wq, struct thread *t);
extern void waitq_enqueue(struct waitq *wq, struct thread *t);

extern void __waitq_remove(struct waitq *wq, struct thread *t);
extern void waitq_remove(struct waitq *wq, struct thread *t);

extern struct thread *__waitq_dequeue(struct waitq *wq);
extern struct thread *waitq_dequeue(struct waitq *wq);

__END_DECLS

#endif /* KERNEL_SCHED_WAITQ_H_ */
