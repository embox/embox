/**
 * @file
 * @brief Kernel worker backed by soft interrupts.
 *
 * @date 13.03.13
 * @author Eldar Abusalimov
 */

#include <kernel/work.h>

#include <kernel/softirq.h>
#include <kernel/irq_lock.h>
#include <embox/unit.h>
#include <util/slist.h>

EMBOX_UNIT_INIT(work_unit_init);

#define WS_PENDING     0x00000fff /* 4096 pending requests, LS bits. */
#define WS_DISABLED    0x0000f000 /* 16 calls to work_disable. */
#define WS_INPROGRESS  0x00010000 /* Being inside the handler. */

#define WS_INIT        0x00000000

/* 01111000 *
 * 01110111 *
 * 00001111 *
 * 00000111 *
 * 00001000 */
#define __COUNT(level) \
	((((level) ^ ((level) - 1)) >> 1) + 1)

static struct slist       workq      = SLIST_INIT(&workq);
static struct slist_link *workq_tail = &workq.sentinel;

typedef member_t(struct work, pending_link) work_pending_t;

static void work_softirq_handler(unsigned int softirq_nr, void *data) {
	struct slist_link *last = &workq.sentinel;

	irq_lock();

	for (struct slist_link *head = &workq.sentinel, *link = head->next;
			link != head; link = link->next) {
		struct work *w = member_to_object(link, work_pending_t);

		if (w->state & WS_DISABLED) {
			last->next = link;
			last = link;
			continue;
		}

		do {
			assert(w->state & WS_PENDING);
			assert(!(w->state & WS_INPROGRESS));

			w->state -= __COUNT(WS_PENDING);
			w->state |= WS_INPROGRESS;

			irq_unlock();
			w->handler(w);
			irq_lock();

			w->state &= ~WS_INPROGRESS;
		} while	(w->state & WS_PENDING);
	}

	last->next = &workq.sentinel;
	workq_tail = last;

	irq_unlock();
}

void work_post(struct work *w) {
	int disabled;

	irq_lock();

	if (!(w->state & (WS_PENDING | WS_INPROGRESS))) {
		struct slist_link *link = member_of_object(w, work_pending_t);
		slist_insert_after_link(link, workq_tail);
		workq_tail = link;
	}

	disabled = w->state & WS_DISABLED;
	w->state += __COUNT(WS_PENDING);

	irq_unlock();

	if (!disabled)
		softirq_raise(SOFTIRQ_NR_WORK);
}

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
	w->state += __COUNT(WS_DISABLED);
	irq_unlock();
}

void work_enable(struct work *w) {
	int pending;

	irq_lock();

	pending = w->state & WS_PENDING;
	w->state -= __COUNT(WS_DISABLED);

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

struct work *work_init(struct work *w, void (*handler)(struct work *),
		unsigned int flags) {

	w->handler = handler;
	w->state = WS_INIT;

	slist_link_init(&w->pending_link);

	if (flags & WORK_F_DISABLED) {
		work_disable(w);
	}

	return w;
}

static int work_unit_init(void) {
	return softirq_install(SOFTIRQ_NR_WORK, work_softirq_handler, NULL);
}
