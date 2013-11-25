/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#include <assert.h>
#include <errno.h>

#include <kernel/irq.h>
#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread.h>

#include <util/member.h>
#include <util/dlist.h>

#include <kernel/critical.h>
#include <kernel/time/timer.h>

void __waitq_enqueue(struct waitq *wq, struct thread *t) {
	struct waitq_link *wql;

	assert(wq && t);
	wql = &t->waitq_link;

	// TODO insert with respect to the priority
	dlist_head_init(&wql->link);
	dlist_add_prev(&wql->link, &wq->list);
}

void waitq_enqueue(struct waitq *wq, struct thread *t) {
	ipl_t ipl;

	assert(wq && t);

	ipl = spin_lock_ipl(&wq->lock);
	__waitq_enqueue(wq, t);
	spin_unlock_ipl(&wq->lock, ipl);
}

void __waitq_remove(struct waitq *wq, struct thread *t) {
	struct waitq_link *wql;

	assert(wq && t);

	wql = &t->waitq_link;
	assert(!dlist_empty(&wql->link));
	dlist_del(&wql->link);
}

void waitq_remove(struct waitq *wq, struct thread *t) {
	ipl_t ipl;

	assert(wq && t);

	ipl = spin_lock_ipl(&wq->lock);
	__waitq_remove(wq, t);
	spin_unlock_ipl(&wq->lock, ipl);
}

struct thread *__waitq_dequeue(struct waitq *wq) {
	struct dlist_head *link;

	assert(wq);

	if (dlist_empty(&wq->list))
		return NULL;

	link = wq->list.next;
	dlist_del(link);

	return dlist_entry(link, struct thread, waitq_link.link);
}

struct thread *waitq_dequeue(struct waitq *wq) {
	struct thread *t;
	ipl_t ipl;

	assert(wq);

	ipl = spin_lock_ipl(&wq->lock);
	t = __waitq_dequeue(wq);
	spin_unlock_ipl(&wq->lock, ipl);

	return t;
}

