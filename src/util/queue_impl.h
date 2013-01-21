/**
 * @file
 * @brief An implementation of 'queue' interface
 *
 * @date 21.01.13
 * @author Ilia Vaprol
 */

#ifndef UTIL_QUEUE_IMPL_H_
#define UTIL_QUEUE_IMPL_H_

#include <stdlib.h>
#include <util/list.h>

struct queue {
	struct list head;
	size_t amount;
};

struct queue_link {
	struct list_link node;
};

#define __QUEUE_INIT(queue) \
	{ \
		.head = LIST_INIT(&(queue)->head), \
		.amount = 0, \
	}

#define __QUEUE_LINK_INIT(link) \
	{ \
		.node = LIST_LINK_INIT(&(link)->node), \
	}

static inline struct queue * queue_init(struct queue *queue) {
	list_init(&queue->head);
	queue->amount = 0;
	return queue;
}

static inline struct queue_link * queue_link_init(struct queue_link *link) {
	list_link_init(&link->node);
	return link;
}

static inline int queue_is_empty(struct queue *queue) {
	return list_is_empty(&queue->head);
}

static inline size_t queue_size(struct queue *queue) {
	return queue->amount;
}

static inline struct queue_link * queue_front_link(struct queue *queue) {
	return list_first_element(&queue->head, struct queue_link, node);
}

static inline struct queue_link * queue_back_link(struct queue *queue) {
	return list_last_element(&queue->head, struct queue_link, node);
}

static inline void queue_push_link(struct queue_link *new_link, struct queue *queue) {
	list_add_last_element(new_link, &queue->head, node);
	++queue->amount;
}

static inline struct queue_link * queue_pop_link(struct queue *queue) {
	struct queue_link *ret;

	if ((ret = member_cast_out_or_null(queue_front_link(queue),
				struct queue_link, node))) {
		list_unlink_element(ret, node);
		--queue->amount;
	}
	return ret;
}

#endif /* UTIL_QUEUE_IMPL_H_ */
