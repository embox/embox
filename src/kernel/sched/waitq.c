/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#include <assert.h>

#include <lib/libds/dlist.h>

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/sched/current.h>

#include <kernel/sched/waitq_protect_link.h>

void waitq_link_init(struct waitq_link *wql) {
	wql->schedee = schedee_get_current();
	dlist_head_init(&wql->link);
}

static void waitq_add(struct waitq *wq, struct waitq_link *wql) {
	ipl_t ipl;

	assert(wq && wql);

	ipl = spin_lock_ipl(&wq->lock);

	if (dlist_empty(&wql->link))
		dlist_add_prev(&wql->link, &wq->list);

	spin_unlock_ipl(&wq->lock, ipl);
}

static void waitq_del(struct waitq *wq, struct waitq_link *wql) {
	ipl_t ipl;

	assert(wq && wql);

	ipl = spin_lock_ipl(&wq->lock);

	if (!dlist_empty(&wql->link))
		dlist_del_init(&wql->link);

	spin_unlock_ipl(&wq->lock, ipl);
}

void __waitq_wait_prepare(struct waitq *wq, struct waitq_link *wql) {
	waitq_add(wq, wql);
}

void waitq_wait_prepare(struct waitq *wq, struct waitq_link *_wql) {
	struct waitq_link *wql;

	wql = waitq_link_create_protected(_wql);

	__waitq_wait_prepare(wq, wql);
	sched_wait_prepare();
}

void __waitq_wait_cleanup(struct waitq *wq, struct waitq_link *wql) {
	waitq_del(wq, wql);
}

void waitq_wait_cleanup(struct waitq *wq, struct waitq_link *_wql) {
	struct waitq_link *wql;

	sched_wait_cleanup();

	wql = waitq_link_find_protected(_wql);

	__waitq_wait_cleanup(wq, wql);

	waitq_link_delete_protected(wql);
}

static void __waitq_wakeup(struct waitq *wq, int nr) {
	struct waitq_link *wql;

	assert(wq);

	dlist_foreach_entry(wql, &wq->list, link) {
		assert(wql->schedee);
		if (!sched_wakeup(wql->schedee))
			continue;

		// TODO mark this wql as the one who has woken the thread up? -- Eldar

		if (!--nr)
			break;
	}
}

void waitq_wakeup(struct waitq *wq, int nr) {
	assert(wq);
	SPIN_IPL_PROTECTED_DO(&wq->lock, __waitq_wakeup(wq, nr));
}
