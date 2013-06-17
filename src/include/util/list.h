/**
 * @file
 * @brief TODO documentation for list.h -- Eldar Abusalimov
 *
 * @date 27.02.11
 * @author Eldar Abusalimov
 * @author Ilia Vaprol
 */

#ifndef UTIL_LIST_H_
#define UTIL_LIST_H_

#include <util/member.h>
#include <util/macro.h>

#if 0
#if defined CONFIG_LIST_NDEBUG || 1
# include __impl_x(util/list_ndebug.h)
#else
# include __impl_x(util/list_debug.h)
#endif
#endif


#include <module/embox/util/List.h>
/* Types used for the list itself and for elements linkage. */

/**
 * A head of the list.
 *
 * @see list_init()
 * @see LIST_INIT()
 * @see LIST_DEF()
 *   A handy macro for defining variables of such type.
 */
struct list;

/**
 * A link which has to be embedded into each element of the list.
 *
 * Usage:
 * @code
 *  struct my_element {
 *  	...
 *  	struct list_link my_link;
 *  	...
 *  };
 * @endcode
 *
 * @see list_link_init()
 * @see LIST_LINK_INIT()
 */
struct list_link;

/* Cast between link member and the element. */

/**
 * Gets a pointer to an element corresponding to a given link.
 *
 * @param link_ptr
 *   The pointer to the member being casted. Must not be null.
 * @param element_t
 *   The type of the element.
 * @param m_link
 *   The name of the link member within the element.
 * @return
 *   @a element_t * pointer.
 */
#define list_element(link_ptr, element_t, m_link) \
	member_cast_out(link_ptr, element_t, m_link)

/* Static initializers. */

/**
 * @param list
 *   Pointer to the list to initialize.
 */
#define LIST_INIT(list) \
	  __LIST_INIT(list)

/**
 * @param link
 *   Pointer to the link.
 */
#define LIST_LINK_INIT(link) \
	  __LIST_LINK_INIT(link)

/**
 * @param list_nm
 *   Variable name to use.
 */
#define LIST_DEF(list_nm) \
	struct list list_nm = LIST_INIT(&list_nm)

/* Initialization functions. */

/**
 * @param list
 *   Pointer to the list being initialized.
 * @return
 *   The argument.
 */
extern struct list *list_init(struct list *list);

/**
 * @param link
 *   Pointer to the link.
 * @return
 *   The argument.
 */
extern struct list_link *list_link_init(struct list_link *link);

extern int list_is_empty(struct list *list);

#define list_alone(linkage_t, element) \
	list_alone_link(member_of_object(element, linkage_t))

#define list_alone_element(element, link_member) \
	list_alone_link(member_cast_in(element, link_member))

extern int list_alone_link(const struct list_link *link);

/* Unlinking an element from its list. */

#define list_unlink(linkage_t, element) \
	list_unlink_link(member_of_object(element, linkage_t))

#define list_unlink_element(element, link_member) \
	list_unlink_link(member_cast_in(element, link_member))

extern void list_unlink_link(struct list_link *link);

/* Retrieving the first/last elements. */

#define list_first(linkage_t, list) \
	member_to_object_or_null(list_first_link(list), linkage_t)

#define list_first_element(list, element_type, link_member) \
	member_cast_out_or_null(list_first_link(list), element_type, link_member)

extern struct list_link *list_first_link(struct list *list);

#define list_last(linkage_t, list) \
	member_to_object_or_null(list_last_link(list), linkage_t)

#define list_last_element(list, element_type, link_member) \
	member_cast_out_or_null(list_last_link(list), element_type, link_member)

extern struct list_link *list_last_link(struct list *list);

/* Adding an element at the list ends. */

#define list_add_first(linkage_t, element, list) \
	list_add_first_link(member_of_object(element, linkage_t), list)

#define list_add_first_element(element, list, link_member) \
	list_add_first_link(member_cast_in(element, link_member), list)

extern void list_add_first_link(struct list_link *new_link, struct list *list);

#define list_add_last(linkage_t, element, list) \
	list_add_last_link(member_of_object(element, linkage_t), list)

#define list_add_last_element(element, list, link_member) \
	list_add_last_link(member_cast_in(element, link_member), list)

extern void list_add_last_link(struct list_link *new_link, struct list *list);

/* Insertion of an element near a given one. */

#define list_insert_before(linkage_t, new_element, element) \
	list_insert_before_link(member_of_object(new_element, linkage_t), \
			member_of_object(element, linkage_t))

#define list_insert_before_element(new_element, element, link_member) \
	list_insert_before_link(member_cast_in(new_element, link_member), \
			member_cast_in(element, link_member))

extern void list_insert_before_link(struct list_link *new_link,
		struct list_link *link);

#define list_insert_after(linkage_t, new_element, element) \
	list_insert_after_link(member_of_object(new_element, linkage_t), \
			member_of_object(element, linkage_t))

#define list_insert_after_element(new_element, element, link_member) \
	list_insert_after_link(member_cast_in(new_element, link_member), \
			member_cast_in(element, link_member))

extern void list_insert_after_link(struct list_link *new_link,
		struct list_link *link);

/* Popping an element from the list ends. */

#define list_remove_first(linkage_t, list) \
	member_to_object_or_null(list_remove_first_link(list), linkage_t)

#define list_remove_first_element(list, element_type, link_member) \
	member_cast_out_or_null(list_remove_first_link(list), element_type, link_member)

extern struct list_link *list_remove_first_link(struct list *list);

#define list_remove_last(linkage_t, list) \
	member_to_object_or_null(list_remove_last_link(list), linkage_t)

#define list_remove_last_element(list, element_type, link_member) \
	member_cast_out_or_null(list_remove_last_link(list), element_type, link_member)

extern struct list_link *list_remove_last_link(struct list *list);

/* Implementing a queue data structure on list. */

#define list_front           list_first
#define list_front_element   list_first_element
#define list_front_link      list_first_link

#define list_back            list_last
#define list_back_element    list_last_element
#define list_back_link       list_last_link

#define list_enqueue         list_add_last
#define list_enqueue_element list_add_last_element
#define list_enqueue_link    list_add_last_link

#define list_dequeue         list_remove_first
#define list_dequeue_element list_remove_first_element
#define list_dequeue_link    list_remove_first_link

/* Implementing a stack data structure on list. */

#define list_top          list_last
#define list_top_element  list_last_element
#define list_top_link     list_last_link

#define list_push         list_add_last
#define list_push_element list_add_last_element
#define list_push_link    list_add_last_link

#define list_pop          list_remove_last
#define list_pop_element  list_remove_last_element
#define list_pop_link     list_remove_last_link

/* Getter of the next element (null if none). */
#define list_next(linkage_t, element, list)                      \
	member_to_object_or_null(                                    \
			list_next_link(member_of_object(element, linkage_t), \
					list),                                       \
				linkage_t)

#define list_next_element(element, list, element_type, link_member) \
	member_cast_out_or_null(                                        \
		list_next_link(member_cast_in(element, link_member), list), \
				element_type, link_member)

extern struct list_link * list_next_link(const struct list_link *link,
		struct list *list);

/* Getter of the previous element (null if none). */
#define list_prev(linkage_t, element, list)                      \
	member_to_object_or_null(                                    \
			list_prev_link(member_of_object(element, linkage_t), \
					list),                                       \
				linkage_t)

#define list_prev_element(element, list, element_type, link_member) \
	member_cast_out_or_null(                                        \
		list_prev_link(member_cast_in(element, link_member), list), \
				element_type, link_member)

extern struct list_link * list_prev_link(struct list_link *link,
		struct list *list);



extern void list_bulk_add_first(struct list *from_list, struct list *to_list);

extern void list_bulk_add_last(struct list *from_list, struct list *to_list);

#define list_bulk_insert_before(from_list, before_element, link_member) \
	list_bulk_insert_before_link(from_list, \
			member_cast_in(before_element, link_member))

extern void list_bulk_insert_before_link(struct list *from_list,
		struct list_link *link);

#define list_bulk_insert_after(from_list, after_element, link_member) \
	list_bulk_insert_after_link(from_list, \
			member_cast_in(after_element, link_member))

extern void list_bulk_insert_after_link(struct list *from_list,
		struct list_link *link);

#define list_foreach_link(link, list) \
	  __list_foreach_link(link, list)

#define list_foreach(element, list, link_member) \
	  __list_foreach(element, list, link_member)

#define __list_foreach_link(link, list) \
	__list_foreach__(link, list, __list_foreach_link_cast_assign, /* unused */)
#define __list_foreach_link_cast_assign(_iter, link, ignored) \
	link = _iter

#define __list_foreach(element, list, m_link) \
	__list_foreach__(element, list, __list_foreach_cast_assign, m_link)
#define __list_foreach_cast_assign(_iter, element, member) \
	element = member_cast_out(_iter, typeof(*element), member)

#define __list_foreach__(node, list, iter_cast, cast_arg) \
	__list_foreach_guarded(node, list, iter_cast, cast_arg, \
			MACRO_GUARD(__iter), \
			MACRO_GUARD(__head), \
			MACRO_GUARD(__next))

#define __list_foreach_guarded(node, list, iter_cast_assign, cast_arg, \
		_iter, _head, _next) \
	for (struct list_link *_head = &__list_check(list)->sentinel, \
				*_iter = (_head)->next, *_next = _iter->next; \
			_iter != _head && (iter_cast_assign(_iter, node, cast_arg)); \
			_iter = _next, _next = _iter->next)

#ifndef __list_check
#define __list_check(expr) (expr)
#endif

#endif /* UTIL_LIST_H_ */
