/**
 * @file
 * @brief Work is a delayed computation unit to perform.
 *
 * @date 13.03.13
 * @author Eldar Abusalimov
 */

#include <kernel/work.h>

#include <hal/ipl.h>
#include <util/dlist.h>
#include <util/member.h>

typedef member_t(struct work, link)       w_link_mt;
typedef member_t(struct work_queue, list) wq_list_mt;

static int __work_enqueue(struct work_queue *wq, struct work *w) {
	int was_alone = dlist_empty(&w->link);

	if (was_alone)
		dlist_add_prev(&w->link, &wq->list);

	return was_alone;
}

int work_enqueue(struct work_queue *wq, struct work *w) {
	return IPL_SAFE_DO(__work_enqueue(wq, w));
}

static struct work *__work_dequeue(struct work_queue *wq) {
	struct dlist_head *first;

	if (dlist_empty(&wq->list))
		return NULL;

	first = wq->list.next;
	dlist_del(first);

	return member_to_object(first, w_link_mt);
}

struct work *work_dequeue(struct work_queue *wq) {
	return IPL_SAFE_DO(__work_dequeue(wq));
}

void work_queue_run(struct work_queue *wq) {
	struct dlist_head *list = member_of_object(wq, wq_list_mt);
	ipl_t ipl;

	ipl = ipl_save();

	for (struct dlist_head *link, *prev = list;
			(link = prev->next) != list; prev = link) {

		struct work *w = member_to_object(link, w_link_mt);
		int handled;

		dlist_del(link);

		ipl_restore(ipl);
		handled = w->handler(w);
		ipl = ipl_save();

		if (!handled && dlist_empty(link))
			dlist_add_next(link, prev);
		else
			link = prev;
	}

	ipl_restore(ipl);
}

#if 0

unsigned int work_pending(struct work *w) {
	return w->state & WS_PENDING;
}
unsigned int work_pending_reset(struct work *w) {
	unsigned int old_state = w->state;
	w->state &= ~WS_PENDING;
	return old_state & WS_PENDING;
}

void work_disable(struct work *w) {
	irq_lock();
	w->state += __WS_COUNT(WS_DISABLED);
	irq_unlock();
}

void work_enable(struct work *w) {
	int pending;

	irq_lock();

	pending = w->state & WS_PENDING;
	w->state -= __WS_COUNT(WS_DISABLED);

	irq_unlock();

	if (pending)
		softirq_raise(SOFTIRQ_NR_WORK);
}

int work_disabled(struct work *w) {
	return w->state & WS_DISABLED;
}

void work_lock(void) {
	softirq_lock();
}
void work_unlock(void) {
	softirq_unlock();
}
#endif

struct work *work_init(struct work *w, void (*handler)(struct work *),
		unsigned int flags) {

	w->handler = handler;
	w->state = WS_INIT;

	dlist_head_init(&w->link);

	// if (flags & WORK_F_DISABLED) {
	// 	work_disable(w);
	// }

	return w;
}

struct work_queue *work_queue_init(struct work_queue *wq) {
	dlist_init(&wq->list);
	return wq;
}

static int work_unit_init(void) {
	return softirq_install(SOFTIRQ_NR_WORK, work_softirq_handler, NULL);
}
