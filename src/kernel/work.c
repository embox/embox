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

#define WS_DISABLED  0x00000fff
#define WS_PENDING   0x00001000

#define WS_INIT      0x00000000

/* 01111000 *
 * 01110111 *
 * 00001111 *
 * 00000111 *
 * 00001000 */
#define __COUNT(level) \
        ((((level) ^ ((level) - 1)) >> 1) + 1)

static void __work_post(struct work *w , struct work_queue *wq) {
	if (!(w->state & WS_PENDING)) {
		w->state |= WS_PENDING;
		w->wq = wq;

		if (!(w->state & WS_DISABLED)) {
			dlist_add_prev(&w->link, &wq->list);
		}
	}
}

static void __work_cancel(struct work *w) {
	if (w->state & WS_PENDING) {
		w->state &= ~WS_PENDING;
		dlist_del(&w->link);
	}
}

static void __work_disable(struct work *w) {
	if (!(w->state & WS_DISABLED) && (w->state & WS_PENDING)) {
		dlist_del(&w->link);
	}

	w->state += __COUNT(WS_DISABLED);
}

static void __work_enable(struct work *w) {
	w->state -= __COUNT(WS_DISABLED);

	if ((w->state & WS_PENDING) && !(w->state & WS_DISABLED)) {
		dlist_add_prev(&w->link, &w->wq->list);
	}
}

void work_init(struct work *w, int (*handler)(struct work *),
		unsigned int flags) {

	w->handler = handler;
	w->wq = NULL;
	w->state = WS_INIT;
	dlist_head_init(&w->link);

	if (flags & WORK_DISABLED) {
		__work_disable(w);
	}
}

void work_queue_init(struct work_queue *wq) {
	dlist_init(&wq->list);
}

void work_post(struct work *w , struct work_queue *wq) {
	IPL_SAFE_DO(__work_post(w, wq));
}

void work_cancel(struct work *w) {
	IPL_SAFE_DO(__work_cancel(w));
}

void work_disable(struct work *w) {
	IPL_SAFE_DO(__work_disable(w));
}

void work_enable(struct work *w) {
	IPL_SAFE_DO(__work_enable(w));
}

void work_queue_run(struct work_queue *wq) {
	struct work *w;
	int handled;

	ipl_t ipl;

	ipl = ipl_save();
	while (!dlist_empty(&wq->list)) {
		dlist_foreach_entry(w, &wq->list, link) {
			dlist_del(&w->link);
			w->state &= ~WS_PENDING;

			ipl_restore(ipl);
			handled = w->handler(w);
			ipl = ipl_save();

			if (!handled) {
				w->state |= WS_PENDING;
				dlist_add_prev(&w->link, &wq->list);
			}
		}
	}

	ipl_restore(ipl);
}
