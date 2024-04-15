/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.10.2013
 */

#ifndef USB_QUEUE_H_
#define USB_QUEUE_H_

#include <lib/libds/dlist.h>

struct usb_queue;
struct usb_queue_link;

struct usb_queue {
	struct dlist_head q;
};

struct usb_queue_link {
	struct dlist_head l;
};

static inline void usb_queue_init(struct usb_queue *q) {
	dlist_init(&q->q);
}

static inline void usb_queue_link_init(struct usb_queue_link *l) {
	dlist_head_init(&l->l);
}

extern int usb_queue_empty(struct usb_queue *q);
extern struct usb_queue_link *usb_queue_last(struct usb_queue *q);
extern struct usb_queue_link *usb_queue_first(struct usb_queue *q);
extern void usb_queue_add(struct usb_queue *q, struct usb_queue_link *l);
extern void usb_queue_del(struct usb_queue *q, struct usb_queue_link *l);

#endif /* USB_QUEUE_H_ */

