/**
 * @file
 *
 * @brief Interface of double linked list
 *
 * @details This interface of double linked list is simular to 'linux list',
 *         but has a few more usage restrictions. It makes this list
 *         more safe. This interface has two implementations: first is ndebug
 *         second is debug. ndebug is simple and has no advantages versus
 *         'linux list', It should be used in release version when the code is
 *         checked and tested. The debug version has more asserts and inspections
 *         during list operations, and it can help you debug the code.
 *
 * @date 19.03.2012
 * @author Anton Bondarev
 */
#ifndef DLIST_H_
#define DLIST_H_

#include <assert.h>

#include <util/member.h>
#include <util/macro.h>

#include <module/embox/lib/libds_dlist.h>

/**
 * A service list data structure. It's used both for elements and for list head.
 * First two members of this data must be 'next' and 'prev' other members can be
 * overloaded in specific implementation (ndebug, debug and so on)
 */
struct dlist_head;

/**
 * A macro for static initializing the list head.
 * This macro must overload in each implementation of dlist interface as
 * #__DLIST_INIT.
 *
 * @param head - head list which wants to be initialized
 */
#define DLIST_INIT(head) \
	__DLIST_INIT(head)

#define DLIST_INIT_NULL() \
	__DLIST_INIT_NULL()

/**
 * A macro declares list head variable and initialize it with #DLIST_INIT. It
 * declare structure of list head if you wand refer to it you should use
 * address of this variable.
 *
 * @param name -variable of a new list head name
 */
#define DLIST_DEFINE(name) \
		struct dlist_head name = DLIST_INIT(name)

/**
 * This function is a runtime analog of #DLIST_INIT macro.
 * You have to use this function if memory for list head is allocated dynamic
 * and you must initialize the head before using (adding an element into the
 * list).
 *
 * @param list_head - head of the list which wants to be initialized before
 *        using
 *
 * @return pointer to the initialized list head
 */
extern struct dlist_head *dlist_init(struct dlist_head *list_head);

/**
 * Initializes item to insert one to a list.
 * This function should use before adding the item to the list if the item was
 * allocated from unspecified place and its memory undefined. This function set
 * item head state to 'init' but not linked state. It can be checked in
 * #dlist_add_next or #dlist_add_prev function.
 *
 * @param item_head - head of the item which wants to be added to a list
 *
 * @return pointer to the initialized item head
 */
extern struct dlist_head *dlist_head_init(struct dlist_head *item_head);

/**
 * Inserts new item to a list after pointed list head.
 * For safe usage of this function you should initialize list head with
 * #dlist_init function or #DLIST_INIT macro and item head with function
 * #dlist_head_init.
 *
 * @param new - head of the item which wants to be inserted into the list
 * @param -list - head of the list which we want to join the new item
 */
extern void dlist_add_next(struct dlist_head *_new, struct dlist_head *list);

/**
 * Inserts new item to a list before pointed list head
 * For safe usage this function you should comply with the same restrictions as
 * in function #dlist_add_next
 *
 * @param new - head of the item which wants to be inserted into the list
 * @param -list - head of the list which we want to join the new item
 */
extern void dlist_add_prev(struct dlist_head *_new,	struct dlist_head *list);

/**
 * Remove item from its list and initialize item head for future using. It
 * links 'next' and 'prev' element in the item list list to each other.
 *
 * @param item_head - deleting item head
 */
extern void dlist_del(struct dlist_head *item_head);
extern void dlist_del_init(struct dlist_head *item_head);

/**
 * Move the item head from the list where it is now to the pointed list.
 *
 * @param head - head of a item which wants to be moved.
 * @param list - new head of the list
 */
static inline void dlist_move(struct dlist_head *head, struct dlist_head *list) {
	dlist_del(head);
	dlist_add_next(head, list);
}

/**
 * Checks whether there are any items in the list except list head or not.
 *
 * @param head - head of the list which wants to be checked
 * @return 1 if list is not empty and zero if empty *
 */
static inline int dlist_empty(const struct dlist_head *head) {
	return (head == head->next);
}

/**
 * @fn dlist_next()
 * @fn dlist_prev()
 *
 * Get next/prev link of a non-empty list.
 */

static inline struct dlist_head *dlist_next(const struct dlist_head *list) {
	assert(!dlist_empty(list));
	return list->next;
}

static inline struct dlist_head *dlist_prev(const struct dlist_head *list) {
	assert(!dlist_empty(list));
	return list->prev;
}

/**
 * @def dlist_next_entry()
 * @def dlist_prev_entry()
 *
 * Get next/prev element of a non-empty list casted to a given type.
 */
#define dlist_next_entry(list, type, member) \
	mcast_out(dlist_next(list), type, member)

#define dlist_prev_entry(list, type, member) \
	mcast_out(dlist_prev(list), type, member)

/**
 * @def dlist_del_init_entry()
 *
 * TODO
 *
 */
#define dlist_del_init_entry(element, link_member) \
		dlist_del_init(member_cast_in(element, link_member))

/**
 * @def dlist_empty_entry()
 *
 * TODO
 *
 */
#define dlist_empty_entry(element, link_member) \
	dlist_empty(member_cast_in(element, link_member))

/**
 * @def dlist_prev_entry_or_null()
 * @def dlist_next_entry_or_null()
 *
 * TODO
 *
 */
#define dlist_prev_entry_or_null(list, element_type, link_member) \
	member_cast_out_or_null(!dlist_empty(list) ? dlist_prev(list) : NULL, \
			element_type, \
			link_member)

#define dlist_next_entry_or_null(list, element_type, link_member) \
	member_cast_out_or_null(!dlist_empty(list) ? dlist_next(list) : NULL, \
			element_type, \
			link_member)

/**
 * @def dlist_next_if_not_last()
 * @def dlist_next_entry_if_not_last()
 *
 * TODO
 *
 */
#define dlist_next_if_not_last(link, list) \
	(link != dlist_prev(list) ? link->next : NULL)

#define dlist_next_entry_if_not_last(element, list, element_type, link_member) \
	member_cast_out_or_null(dlist_next_if_not_last(member_cast_in(element, \
	link_member), list), element_type, link_member)

/**
 * @def dlist_add_prev_entry()
 * @def dlist_add_next_entry() TODO
 *
 * TODO
 *
 */
#define dlist_add_prev_entry(element, list, link_member) \
	dlist_add_prev(member_cast_in(element, link_member), list)

/**
 * @fn dlist_first()
 * @fn dlist_last()
 *
 * Get first/last link of a non-empty list.
 */
#define dlist_first dlist_next
#define dlist_last  dlist_prev

/**
 * @fn dlist_first_or_null()
 * @fn dlist_last_or_null()
 *
 * Get first/last link with null as a fallback return value
 * for case of an empty list.
 */

static inline struct dlist_head *dlist_first_or_null(const struct dlist_head *list) {
	return (!dlist_empty(list) ? dlist_first(list) : NULL);
}

static inline struct dlist_head *dlist_last_or_null(const struct dlist_head *list) {
	return (!dlist_empty(list) ? dlist_last(list) : NULL);
}

/**
 * @def dlist_first_entry()
 * @def dlist_last_entry()
 *
 * Get first/last element of a non-empty list casted to a given type.
 */
#define dlist_first_entry dlist_next_entry
#define dlist_last_entry  dlist_prev_entry

/**
 * @def dlist_first_entry_or_null()
 * @def dlist_last_entry_or_null()
 *
 * Get first/last element casted to a given type. A list may be empty.
 */

#define dlist_first_entry_or_null(list, type, member) \
	mcast_out_or_null(dlist_first_or_null(list), type, member)

#define dlist_last_entry_or_null(list, type, member) \
	mcast_out_or_null(dlist_last_or_null(list), type, member)

/**
 * Receive the element structure which encapsulates item head.
 *
 * @param head - pointer to the item head in demanded structure
 * @param type - type of the element structure
 * @param member - name of the head field in the element structure
 * @return a pointer to the item structure which contains the head
 */
#define dlist_entry(head, type, member) \
    mcast_out(head, type, member)

/*
 * 'for'-like loops safe to modification from inside a loop body.
 *
 * An expression of 'head' is always evaluated exactly once.
 * An iteration variable is not touched in case of an empty list.
 * Otherwise it holds the element of the last iteration.
 * Loop body may overwrite it with no effects.
 */

#define dlist_foreach       dlist_foreach_safe
#define dlist_foreach_entry dlist_foreach_entry_safe

#define dlist_foreach_safe(link, head) \
	__dlist_foreach_safe(link, head, \
		MACRO_GUARD(__link), \
		MACRO_GUARD(__head), \
		MACRO_GUARD(__next))

#define __dlist_foreach_safe(link, head, __link, __head, __next) \
	for (struct dlist_head *__link,       \
			*__head = (head),              \
			*__next = __head->next;        \
			                              \
		__next = (__link = __next)->next, \
			(__link != __head) &&         \
			((link = __link), 1);)


#define dlist_foreach_entry_safe(link, head, member) \
	__dlist_foreach_entry_safe(link, head, member, \
		MACRO_GUARD(__link), \
		MACRO_GUARD(__head), \
		MACRO_GUARD(__next))

#define __dlist_foreach_entry_safe(link, head, member, __link, __head, __next) \
	for (struct dlist_head *__link,                                 \
			*__head = (head),                                       \
			*__next = (assert(__head), __head->next);               \
		__next = (assert(__next), __link = __next)->next,           \
			(__link != __head) &&                                   \
			(link = dlist_entry(__link, typeof(*link), member), 1); \
		)


#endif /* DLIST_H_ */
