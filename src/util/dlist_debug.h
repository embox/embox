/**
 * @file
 *
 * @brief An safe implementation of dlist interface. It protects the list from
 * re-add and re-delete an item.
 *
 *
 *
 * @date 03.04.2012
 * @author Anton Bondarev
 */

#ifndef DLIST_DEBUG_H_
#define DLIST_DEBUG_H_

#include <assert.h>
/**
 * Double list head with an additional field 'list_id'. It contains address of
 * list head (entry of the list) structure or NULL if item isn't initialized
 * now.
 */
struct dlist_head {
	struct dlist_head *next;   /**<pointer to next item in the list*/
	struct dlist_head *prev;   /**<pointer to previous item in the list*/
	/**
	 * Special implementation field. It's used as flag to indicate whether the
	 * head is in a list or not
	 */
	struct dlist_head *list_id;
};
/** The implementation of the #DLIST_INIT macro */
#define __DLIST_INIT(head) { &(head), &(head), &(head) }

/* Only for internal using */
static inline void __dlist_add(struct dlist_head *new, struct dlist_head *next,
		struct dlist_head *prev) {
	new->prev = prev;
	new->next = next;
	next->prev = new;
	prev->next = new;
}

/* Checks whether item or list head is owned some of double lists or not.
 * Only for internal using
 */
static inline int __is_linked(struct dlist_head *head) {
	return (NULL != head->list_id);
}
/**
 * Initialize the item head. Set list_is as NULL it means that item head is not
 * in any list
 */
static inline void dlist_head_init(struct dlist_head *head) {
	head->next = head;
	head->prev = head;
	head->list_id = NULL;
}

/**
 * Initialize the list head. Setup all fields as own address, it means that
 * the list is looped itself (marked empty), and list_id field with own address
 * points follows this head now is owned the list and the list entry is this
 * head.
 */
static inline void dlist_init(struct dlist_head *head) {
	head->list_id = head->next = head->prev = head;
}

static inline void dlist_add_next(struct dlist_head *new,
		struct dlist_head *list) {

	assert(!__is_linked(new)); /* we can't add not initialized element*/

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

	assert(!__is_linked(new)); /* we can't add not initialized element */

	if (!__is_linked(list)) {
		list->list_id = list;
		new->list_id = list;
	} else {
		new->list_id = list->list_id;
	}

	__dlist_add(new, list, list->prev);
}

/**
 * Remove the element from its list. First of all it checks whether the head
 * is owned any of list, if head not in the list it is error. If head is owned
 * the list (list_id is setup in correct value) the head will unlink from the
 * list (previous will point to the next item and next will point to the
 * previous). And at the end the head is marked 'initialize' (not in a list)
 */
static inline void dlist_del(struct dlist_head *head) {
	assert(__is_linked(head)); /* we can't remove initialized element */

	head->prev->next = head->next;
	head->next->prev = head->prev;

	head->list_id = NULL; /* dlist_head_init */
}

#endif /* DLIST_DEBUG_H_ */
