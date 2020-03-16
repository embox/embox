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

static struct usb_queue_link *usb_queue_link(struct usb_queue *q,
		struct dlist_head *l) {

	if (dlist_empty(&q->q)) {
		return NULL;
	}

	return member_cast_out(l, struct usb_queue_link, l);
}

struct usb_queue_link *usb_queue_first(struct usb_queue *q) {
	return IRQ_LOCKED_DO(usb_queue_link(q, q->q.next));
}

struct usb_queue_link *usb_queue_last(struct usb_queue *q) {
	return IRQ_LOCKED_DO(usb_queue_link(q, q->q.prev));
}

void usb_queue_add(struct usb_queue *q, struct usb_queue_link *l) {
	irq_lock();
	{
		dlist_head_init(&l->l);
		dlist_add_prev(&l->l, &q->q);
	}
	irq_unlock();
}

void usb_queue_del(struct usb_queue *q, struct usb_queue_link *l) {
	IRQ_LOCKED_DO(dlist_del(&l->l));
}

int usb_queue_empty(struct usb_queue *q) {
	return IRQ_LOCKED_DO(dlist_empty(&q->q));
}
