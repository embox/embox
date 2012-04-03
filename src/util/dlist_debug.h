/**
 * @file
 *
 * @brief An safe implementation of dlist interface. It save the list by
 *       double adding or deleting the same item.
 *
 *
 * @date 03.04.2012
 * @author Anton Bondarev
 */

#ifndef DLIST_DEBUG_H_
#define DLIST_DEBUG_H_

#include <assert.h>

struct dlist_head {
	struct dlist_head *next;
	struct dlist_head *prev;
	struct dlist_head *list_id;
};

#define __DLIST_INIT(head) { &(head), &(head), &(head) }

/* only for internal using */
static inline void __dlist_add(struct dlist_head *new, struct dlist_head *next,
		struct dlist_head *prev) {
	new->prev = prev;
	new->next = next;
	next->prev = new;
	prev->next = new;
}

static inline int __is_linked(struct dlist_head *head) {
	return (NULL != head->list_id);
}

static inline void dlist_head_init(struct dlist_head *head) {
	head->list_id = NULL;
}

static inline void dlist_init(struct dlist_head *head) {
	head->list_id = head->next = head->prev = head;
}

static inline void dlist_add_next(struct dlist_head *new,
		struct dlist_head *list) {

	assert(__is_linked(new)); /* we can't add not initialized element*/

	if (!__is_linked(list)) {
		list->list_id = list;
		new->list_id = list;
	} else {
		new->list_id = list->list_id;
	}

	__dlist_add(new, list->next, list);
}

static inline void dlist_add_prev(struct dlist_head *new,
		struct dlist_head *list) {

	assert(__is_linked(new)); /* we can't add not initialized element */

	if (!__is_linked(list)) {
		list->list_id = list;
		new->list_id = list;
	} else {
		new->list_id = list->list_id;
	}

	__dlist_add(new, list, list->prev);
}

static inline void dlist_del(struct dlist_head *head) {
	assert(!__is_linked(head)); /* we can't remove initialized element */

	head->prev->next = head->next;
	head->next->prev = head->prev;

	head->list_id = NULL; /* dlist_head_init */
}

#endif /* DLIST_DEBUG_H_ */
