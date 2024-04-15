/**
 * @file
 * @brief TODO documentation for slist.h -- Eldar Abusalimov
 *
 * @date 16.04.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_SLIST_H_
#define UTIL_SLIST_H_

#include <util/member.h>

struct slist_link {
	struct slist_link *next;
	/*struct __slist_debug debug;*/
};

struct slist {
	struct slist_link sentinel;
};

#define SLIST_LINK_INIT(link) \
	{ \
		.next  = (link), \
		/*.debug = __SLIST_DEBUG_LINK_INIT(link),*/ \
	}

#define SLIST_INIT(slist) \
	{ \
		.sentinel = { \
			.next  = &(slist)->sentinel, \
			/*.debug = __SLIST_DEBUG_INIT(slist),*/ \
		} \
	}

#define slist_element(link_ptr, element_t, m_link) \
	member_cast_out(link_ptr, element_t, m_link)

static inline struct slist_link *slist_link_init(struct slist_link *link) {
	// __slist_debug_link_init(link);
	link->next = link;
	return link;
}

static inline struct slist *slist_init(struct slist *list) {
	// __slist_debug_init(list);
	list->sentinel.next = &list->sentinel;
	return list;
}

static inline int slist_empty(struct slist *list) {
	// __slist_debug_empty(list);
	return list->sentinel.next == &list->sentinel;
}

static inline int slist_alone_link(struct slist_link *link) {
	// __slist_debug_alone(link);
	return link->next == link;
}
#define slist_alone(element, member_t) \
	slist_alone_link(member_of_object(element, member_t))
#define slist_alone_element(element, link_member) \
	slist_alone_link(member_cast_in(element, link_member))

static inline struct slist_link *slist_first_link(struct slist *list) {
	// __slist_debug_first(list);
	return slist_empty(list) ? NULL : list->sentinel.next;
}
#define slist_first(slist, member_t) \
	member_to_object_or_null(slist_first_link(slist), member_t)
#define slist_first_element(slist, element_type, link_member) \
	member_cast_out_or_null(slist_first_link(slist), element_type, link_member)

static inline void slist_insert_after_link(struct slist_link *new_link,
		struct slist_link *link) {
	// __slist_debug_insert_after(new_link, list);
	new_link->next = link->next;
	link->next = new_link;
}
#define slist_insert_after(element, after_element, member_t) \
	slist_insert_after_link(member_of_object(element, member_t), \
		member_of_object(after_element, member_t))
#define slist_insert_after_element(element, after_element, link_member) \
	slist_insert_after_link(member_cast_in(element, link_member), \
			member_cast_in(after_element, link_member))

static inline void slist_add_first_link(struct slist_link *new_link,
		struct slist *list) {
	slist_insert_after_link(new_link, &list->sentinel);
}
#define slist_add_first(element, slist, member_t) \
	slist_add_first_link(member_of_object(element, member_t), slist)
#define slist_add_first_element(element, slist, link_member) \
	slist_add_first_link(member_cast_in(element, link_member), slist)

static inline struct slist_link *slist_remove_first_link(struct slist *list) {
	struct slist_link *first;
	// __slist_debug_remove_first(list);
	first = slist_first_link(list);
	if (first) {
		list->sentinel.next = first->next;
		slist_link_init(first);
	}
	return first;
}

#define slist_remove_first(slist, member_t) \
	member_to_object_or_null(slist_remove_first_link(slist), member_t)
#define slist_remove_first_element(slist, element_type, link_member) \
	member_cast_out_or_null(slist_remove_first_link(slist), element_type, \
			link_member)

#define slist_foreach_link(link, slist) \
	  __slist_foreach_link(link, slist)
#define slist_foreach(element, slist, link_member) \
	  __slist_foreach(element, slist, link_member)

#define __slist_foreach_cast_assign(_iter, element, member) \
	element = member_cast_out(_iter, typeof(*element), member)

#define __slist_foreach(element, slist, m_link) \
	__slist_foreach__(element, slist, __slist_foreach_cast_assign, m_link)

#define __slist_foreach__(node, slist, iter_cast, cast_arg) \
	__slist_foreach_guarded(node, slist, iter_cast, cast_arg, \
			MACRO_GUARD(__iter), \
			MACRO_GUARD(__head), \
			MACRO_GUARD(__next))

#define __slist_foreach_guarded(node, slist, iter_cast_assign, cast_arg, \
		_iter, _head, _next) \
	for (struct slist_link *_head = &(slist)->sentinel, \
				*_iter = (_head)->next, *_next = _iter->next; \
			_iter != _head && (iter_cast_assign(_iter, node, cast_arg)); \
			_iter = _next, _next = _iter->next)


#endif /* UTIL_SLIST_H_ */
