/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.10.2013
 */

#include <stdbool.h>
#include <kernel/irq_lock.h>
#include <util/member.h>

#include <drivers/usb/usb_queue.h>

struct usb_queue_link *usb_queue_peek(struct usb_queue *q) {
	struct dlist_head *l = q->q.next;

	if (dlist_empty(&q->q)) {
		return NULL;
	}

	return member_cast_out(l, struct usb_queue_link, l);
}

int usb_queue_add(struct usb_queue *q, struct usb_queue_link *l) {
	bool is_empty;

	irq_lock();
	{
		is_empty = dlist_empty(&q->q);
		dlist_head_init(&l->l);
		dlist_add_prev(&l->l, &q->q);
	}
	irq_unlock();

	return !is_empty;
}

int usb_queue_done(struct usb_queue *q, struct usb_queue_link *l) {
	bool is_empty;

	irq_lock();
	{
		struct usb_queue_link *cl = usb_queue_peek(q);

		assert(q->q.next == &l->l);
		assert(cl == l);

		dlist_del(&l->l);

		is_empty = dlist_empty(&q->q);
	}
	irq_unlock();

	return !is_empty;
}
