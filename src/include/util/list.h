/**
 * @file
 * @brief TODO documentation for list.h -- Eldar Abusalimov
 *
 * @date 27.02.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_H_
#define UTIL_LIST_H_

#include <util/adt.h>
#include <util/member.h>
#include <util/macro.h>

//#include __impl_x(util/list_impl.h)

struct __list_debug { };

struct list_link {
	struct list_link *next;
	struct list_link *prev;
	struct __list_debug debug;
};

struct list {
	struct list_link sentinel;
};

#define list_element_t(element_type, link_member) \
	adt_element_t(element_type, link_member)

#define list_element(link, element_type, link_member) \
	member_out(link, element_type, link_member)

#define LIST_DEF(list_nm) \
	struct list list_nm = LIST_INIT(&list_nm)

#define LIST_INIT(list) \
	{ \
		.sentinel = LIST_LINK_INIT(&(list)->sentinel), \
	}

#define LIST_LINK_INIT(link) \
	{ \
		.next = (link), \
		.prev = (link), \
	}
//		.debug = __LIST_DEBUG_INIT(link),

static inline void __list_bind(struct list_link *prev,
		struct list_link *next) {
	next->prev = prev;
	prev->next = next;
}

static inline struct list_link *list_link_init(struct list_link *link) {
	__list_bind(link, link);
	return link;
}

static inline struct list *list_init(struct list *list) {
	list_link_init(&list->sentinel);
	return list;
}

#define list_alone_element(list_type, element) \
	list_alone_link(adt_to_link(element, list_type))

#define list_alone(element, link_member) \
	list_alone_link(member_in(element, link_member))

static inline int list_alone_link(struct list_link *link) {
	return link == link->next;
}

static inline int list_is_empty(struct list *list) {
	return list_alone_link(&list->sentinel);
}

#define list_first(list, element_type, link_member) \
	member_out_or_null(list_first_link(list), element_type, link_member)

static inline struct list_link *list_first_link(struct list *list) {
	struct list_link *sentinel = &list->sentinel, *first = sentinel->next;
	return first != sentinel ? first : NULL;
}

#define list_last(list, element_type, link_member) \
	member_out_or_null(list_last_link(list), element_type, link_member)

static inline struct list_link *list_last_link(struct list *list) {
	struct list_link *sentinel = &list->sentinel, *last = sentinel->prev;
	return last != sentinel ? last : NULL;
}

#define list_add_first(element, list, link_member) \
	list_add_first_link(member_in(element, link_member), list)

static inline void list_add_first_link(struct list_link *new_link,
		struct list *list) {
	struct list_link *sentinel = &list->sentinel;
	__list_bind(new_link, sentinel->next);
	__list_bind(sentinel, new_link);
}

#define list_add_last(element, list, link_member) \
	list_add_last_link(member_in(element, link_member), list)

static inline void list_add_last_link(struct list_link *new_link,
		struct list *list) {
	struct list_link *sentinel = &list->sentinel;
	__list_bind(sentinel->prev, new_link);
	__list_bind(new_link, sentinel);
}

#define list_insert_before(new_element, element, link_member) \
	list_insert_before_link(member_in(new_element, link_member), \
			member_in(element, link_member))

static inline void list_insert_before_link(struct list_link *new_link,
		struct list_link *link) {
	__list_bind(link->prev, new_link);
	__list_bind(new_link, link);
}

#define list_insert_after(new_element, element, link_member) \
	list_insert_after_link(member_in(new_element, link_member), \
			member_in(element, link_member))

static inline void list_insert_after_link(struct list_link *new_link,
		struct list_link *link) {
	__list_bind(new_link, link->next);
	__list_bind(link, new_link);
}

#define list_remove(element, link_member) \
	list_remove_link(member_in(element, link_member))

static inline void list_remove_link(struct list_link *link) {
	__list_bind(link->prev, link->next);
	list_link_init(link);
}

#define list_remove_first(list, element_type, link_member) \
	member_out_or_null(list_remove_first_link(list), element_type, link_member)

static inline struct list_link *list_remove_first_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_first_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}

#define list_remove_last(list, element_type, link_member) \
	member_out_or_null(list_remove_last_link(list), element_type, link_member)

static inline struct list_link *list_remove_last_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_last_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}

static inline void __list_bulk_insert_between(struct list *from_list,
		struct list_link *prev, struct list_link *next) {
	if (!list_is_empty(from_list)) {
		struct list_link *from = &from_list->sentinel;
		struct list_link *first = from->next, *last = from->prev;

		__list_bind(prev, first);
		__list_bind(last, next);

		list_link_init(from);
	}
}

static inline void list_bulk_add_first(struct list *from_list,
		struct list *to_list) {
	struct list_link *to = &to_list->sentinel;
	__list_bulk_insert_between(from_list, to, to->next);
}

static inline void list_bulk_add_last(struct list *from_list,
		struct list *to_list) {
	struct list_link *to = &to_list->sentinel;
	__list_bulk_insert_between(from_list, to->prev, to);
}

#define list_bulk_insert_before(from_list, before_element, link_member) \
	list_bulk_insert_before_link(from_list, \
			member_in(before_element, link_member))

static inline void list_bulk_insert_before_link(struct list *from_list,
		struct list_link *link) {
	__list_bulk_insert_between(from_list, link->prev, link);
}

#define list_bulk_insert_after(from_list, after_element, link_member) \
	list_bulk_insert_after_link(from_list, \
			member_in(after_element, link_member))

static inline void list_bulk_insert_after_link(struct list *from_list,
		struct list_link *link) {
	__list_bulk_insert_between(from_list, link, link->next);
}

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
	element = member_out(_iter, typeof(*element), member)

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

