/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.10.2013
 */

#ifndef USB_QUEUE_H_
#define USB_QUEUE_H_

#include <util/dlist.h>

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

struct usb_queue_link *usb_queue_last(struct usb_queue *q);

struct usb_queue_link *usb_queue_peek(struct usb_queue *q);

/**
 * @brief Add link to the tail of queue
 *
 * @param q
 * @param l
 *
 * @return true if queue was not empty
 * @return false if queue was empty and \a q is the first
 */
int usb_queue_add(struct usb_queue *q, struct usb_queue_link *l);

/**
 * @brief Pop current link form queue
 *
 * @param q
 * @param l
 *
 * @return true if queue is not empty
 * @return false if queue is not empty
 */
int usb_queue_done(struct usb_queue *q, struct usb_queue_link *l);

/**
 * @brief Remove link from queue
 *
 * @param q
 * @param l
 *
 * @return true if it was first link in queue
 * @return false in other case
 */
int usb_queue_remove(struct usb_queue *q, struct usb_queue_link *l);

#endif /* USB_QUEUE_H_ */

