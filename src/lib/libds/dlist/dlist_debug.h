/**
 * @file
 *
 * @brief An safe implementation of 'dlist' interface. It protects the list
 *      from re-add and re-delete an item.
 *
 * @details This implementation of 'dlist' interface uses special value for
 *       examine re-add and re-delete operation. This value is list_id. It is
 *       the list head address. Each item head in the list must contains
 *       similar list_id. If adding item head has set not NULL list_id field it
 *       means that this item head already in a list and the assertion will
 *       call.
 *
 * @date 03.04.2012
 *
 * @author Anton Bondarev
 */

#ifndef DLIST_DEBUG_H_
#define DLIST_DEBUG_H_

#include <stddef.h>
#include <stdint.h>
// #include <assert.h> /*it's required for assertion operation */

#include <framework/mod/options.h>

#define DLIST_DEBUG_VERSION OPTION_MODULE_GET(embox__lib__libds_dlist, NUMBER, debug)
/**
 * Double list head structure with an additional field 'list_id'. It contains
 * an address of a list head (entry of the list) structure or NULL if item is
 * not initialized now.
 */
struct dlist_head {
	struct dlist_head *next;   /**<pointer to next item in the list*/
	struct dlist_head *prev;   /**<pointer to previous item in the list*/

#if DLIST_DEBUG_VERSION
	uintptr_t          poison; /**< Valid value is zero or ~&head. */
	/**
	 * Special implementation field. It's used as flag to indicate whether the
	 * head is in a list or not
	 */
	struct dlist_head *list_id;
#endif
};

#if DLIST_DEBUG_VERSION
/** The implementation of the #DLIST_INIT macro */
#define __DLIST_INIT(head) { &(head), &(head), 0, &(head) }

#define __DLIST_INIT_NULL() { NULL, NULL, 0, NULL }

extern void __dlist_debug_check(const struct dlist_head *head);

/* Only for internal using */
static inline void __dlist_add(struct dlist_head *_new, struct dlist_head *next,
		struct dlist_head *prev) {
	__dlist_debug_check(prev);
	__dlist_debug_check(next);
	_new->prev = prev;
	_new->next = next;

	_new->poison = ~(uintptr_t) _new;
	next->prev = _new;
	prev->next = _new;
	__dlist_debug_check(_new);
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
static inline struct dlist_head *dlist_head_init(struct dlist_head *head) {
	head->next = head->prev = head;
	head->poison = ~(uintptr_t) head;
	head->list_id = NULL; /* mark it's not in a list */

	return head;
}

/**
 * Implementation of the #dlist_init function.
 * It initializes the list head.
 * Setup all fields as own address, it means that the list is looped itself
 * (marked empty), and list_id field filled with the own address points.
 * It means that this list head now is owned the specify list and the list id
 * is address of this list head.
 */
static inline struct dlist_head *dlist_init(struct dlist_head *head) {
	head->next = head->prev = head; /* closure list */
	head->poison = ~(uintptr_t) head;
	head->list_id = head; /* mark it's in a list and this head is list entry */

	return head;
}

/**
 * Implementation of the #dlist_add_next function
 * First of all it examine correct state of the item head and the list head.
 * List head must be in a list but item head not.
 * Then if state is correct it marks the new item head as owned this list.
 * And at the and it add the new element into the list after list head element.
 */
static inline void dlist_add_next(struct dlist_head *_new,
		struct dlist_head *list) {
	/* we can't add not initialized element.
	 * use #dlist_head_init before using _new element*/
	// assert(!__is_linked(_new)); /* re-add element */
	/* we can't use list head without initialization.
	* Use macro #DLIST_INIT for static or #dlist_init for dynamic
	* initialization */
	// assert(__is_linked(list)); /* add to not initialized list */

	__dlist_debug_check(list);
	_new->list_id = list->list_id; /* mark item head as added to this list */

	/* Real adding the element
	 * _new sequence will be following
	 * list head <-> _new item <-> next item of list head
	 */
	__dlist_add(_new, list->next, list);
}

/**
 * Implementation of the #dlist_add_prev function
 * First of all it examine correct state of the item head and the list head.
 * List head must be in a list but item head not.
 * Then if state is correct it marks the _new item head as owned this list.
 * And at the and it add the _new element into the list as previous element for
 * the list head.
 */
static inline void dlist_add_prev(struct dlist_head *_new,
		struct dlist_head *list) {
	/* we can't add not initialized element.
	 * use #dlist_head_init before using new element*/
	// assert(!__is_linked(_new)); /* re-add element */
	/* we can't use list head without initialization.
	 * Use macro #DLIST_INIT for static or #dlist_init for dynamic
	 * initialization */
	// assert(__is_linked(list)); /* add to not initialized list */

	__dlist_debug_check(list);
	_new->list_id = list->list_id; /* mark item head as added to this list */

	/* Real adding the element
	 * new sequence will be following
	 * list head <-> _new item <-> previous item of list head
	 */
	__dlist_add(_new, list, list->prev);
}

/**
 * Remove the element from its list. First of all it checks whether the head
 * is owned any of list, if head not in the list it is error. If head is owned
 * the list (list_id is setup in correct value) the head will unlink from the
 * list (previous will point to the next item and next will point to the
 * previous). And at the end the head is marked 'initialize' (not in a list)
 */
static inline void dlist_del(struct dlist_head *head) {
	// assert(__is_linked(head)); /* we can't remove initialized element */

	__dlist_debug_check(head);
	/* close the list
	 * the previous element refer to the next element and next element refer to
	 * the previous */
	head->prev->next = head->next;
	head->next->prev = head->prev;

	head->list_id = NULL; /* dlist_head_init */
}

static inline void dlist_del_init(struct dlist_head *head) {
	dlist_del(head);
	dlist_init(head);
}
#else /* !DLIST_DEBUG_VERSION */

/** The implementation of the #DLIST_INIT macro */
#define __DLIST_INIT(head) { &(head), &(head)}

#define __DLIST_INIT_NULL(head) { NULL, NULL}

extern void __dlist_debug_check(const struct dlist_head *head);

/* Only for internal using */
static inline void __dlist_add(struct dlist_head *_new, struct dlist_head *next,
		struct dlist_head *prev) {

	_new->prev = prev;
	_new->next = next;

	next->prev = _new;
	prev->next = _new;
}

/**
 * Initialize the item head. Set list_is as NULL it means that item head is not
 * in any list
 */
static inline struct dlist_head *dlist_head_init(struct dlist_head *head) {
	head->next = head->prev = head;
	return head;
}

/**
 * Implementation of the #dlist_init function.
 * It initializes the list head.
 * Setup all fields as own address, it means that the list is looped itself
 * (marked empty), and list_id field filled with the own address points.
 * It means that this list head now is owned the specify list and the list id
 * is address of this list head.
 */
static inline struct dlist_head *dlist_init(struct dlist_head *head) {
	head->next = head->prev = head; /* closure list */

	return head;
}

/**
 * Implementation of the #dlist_add_next function
 * First of all it examine correct state of the item head and the list head.
 * List head must be in a list but item head not.
 * Then if state is correct it marks the new item head as owned this list.
 * And at the and it add the new element into the list after list head element.
 */
static inline void dlist_add_next(struct dlist_head *_new,
		struct dlist_head *list) {
	/* Real adding the element
	 * _new sequence will be following
	 * list head <-> _new item <-> next item of list head
	 */
	__dlist_add(_new, list->next, list);
}

/**
 * Implementation of the #dlist_add_prev function
 * First of all it examine correct state of the item head and the list head.
 * List head must be in a list but item head not.
 * Then if state is correct it marks the _new item head as owned this list.
 * And at the and it add the _new element into the list as previous element for
 * the list head.
 */
static inline void dlist_add_prev(struct dlist_head *_new,
		struct dlist_head *list) {

	/* Real adding the element
	 * new sequence will be following
	 * list head <-> _new item <-> previous item of list head
	 */
	__dlist_add(_new, list, list->prev);
}

/**
 * Remove the element from its list. First of all it checks whether the head
 * is owned any of list, if head not in the list it is error. If head is owned
 * the list (list_id is setup in correct value) the head will unlink from the
 * list (previous will point to the next item and next will point to the
 * previous). And at the end the head is marked 'initialize' (not in a list)
 */
static inline void dlist_del(struct dlist_head *head) {
	/* close the list
	 * the previous element refer to the next element and next element refer to
	 * the previous */
	head->prev->next = head->next;
	head->next->prev = head->prev;
}

static inline void dlist_del_init(struct dlist_head *head) {
	dlist_del(head);
	dlist_init(head);
}

#endif /* DLIST_DEBUG_VERSION */

#endif /* DLIST_DEBUG_H_ */
