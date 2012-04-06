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

#if defined CONFIG_LIST_NDEBUG || 0
# include __impl_x(util/dlist_ndebug.h)
#else
# include __impl_x(util/dlist_debug.h)
#endif

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
 */
#define DLIST_INIT(head) \
	__DLIST_INIT(head)

/**
 * A macro declares list head variable and initialize it with #DLIST_INIT.
 */
#define DLIST_DEFINE(name) \
		struct dlist_head name = DLIST_INIT(name)

/**
 * This function is a runtime analog of #DLIST_INIT macro.
 * You have to use this function if memory for list head is allocated dynamic
 * and you must initialize the head before using (adding an element into the
 * list).
 */
extern void dlist_init(struct dlist_head *head);

/**
 * Initializes item to insert one to a list.
 * This function should use before adding the item to the list if the item was
 * allocated from unspecified place and its memory undefined. This function set
 * item head state to 'init' but not linked state. It can be checked in
 * #dlist_add_next or #dlist_add_prev function.
 */
extern struct dlist_head *dlist_head_init(struct dlist_head *head);

/**
 * Inserts new item to a list after pointed list head.
 * For safe usage of this function you should initialize list head with
 * #dlist_init function or #DLIST_INIT macro and item head with function
 * #dlist_head_init.
 */
extern void dlist_add_next(struct dlist_head *new, struct dlist_head *list);

/**
 * Inserts new item to a list before pointed list head
 * For safe usage this function you should comply with the same restrictions as
 * in function #dlist_add_next
 */
extern void dlist_add_prev(struct dlist_head *new,	struct dlist_head *list);

/**
 * Remove item from its list and initialize item head for new using.
 */
extern void dlist_del(struct dlist_head *head);

/**
 * move item from the list where it places now to the pointed list
 */
static inline void dlist_move(struct dlist_head *head, struct dlist_head *list) {
	dlist_del(head);
	dlist_add_next(head, list);
}

/**
 * Checks whether there are any items in the list or not.
 */
static inline int dlist_empty(struct dlist_head *head) {
	return head == head->next;
}

/**
 * Return a pointer to the item structure which contains the head
 */
#define dlist_entry(head, type, member) \
    ((type *)((char *)(head) - (unsigned long)(&((type *)0)->member)))


#define dlist_foreach(ptr, nxt, head) \
	ptr = (head)->next; nxt = ptr->next;                         \
	for (; ptr != (head); ptr = nxt, nxt = ptr->next)


#define dlist_foreach_entry(ptr, nxt, head, member)  \
	ptr = dlist_entry((head)->next, typeof(*ptr), member);         \
	nxt = dlist_entry(ptr->member.next, typeof(*ptr), member);     \
	for (; &ptr->member != (head);                                 \
		ptr = nxt,                                                 \
		nxt = dlist_entry(nxt->member.next, typeof(*ptr), member)) \

#endif /* DLIST_H_ */
