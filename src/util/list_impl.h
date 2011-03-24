/**
 * @file
 * @brief TODO documentation for list.h -- Eldar Abusalimov
 *
 * @date Feb 27, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_IMPL_H_
#define UTIL_LIST_IMPL_H_

#include <assert.h>
#include <util/structof.h>

struct list_link {
	struct list_link *next, *prev;
};

struct list {
	struct list_link link;
};

#define __LIST_INIT(list) { \
		.link = LIST_LINK_INIT(&(list)->link) \
	}

#define __LIST_LINK_INIT(link) { \
		.next = (link), \
		.prev = (link)  \
	}

/* Most of macros are defined through a corresponding _link method. */

#define __list_link_element(link, type, m_link) \
	__extension__ ({ \
		assert((link) != NULL);       \
		structof(link, type, m_link); \
	})

#define __list_alone(element, m_link) \
	list_alone_link(&(element)->m_link)

#define __list_first(list, type, m_link) \
	__list_link_element_check(list_first_link(list), type, m_link)

#define __list_last(list, type, m_link) \
	__list_link_element_check(list_last_link(list), type, m_link)

#define __list_link_element_check(link, type, m_link) \
	__extension__ ({ \
		struct list_link *__list_link__ = (link); \
		__list_link__ ? list_link_element(__list_link__, type, m_link) : NULL;\
	})

#define __list_add_first(element, list, m_link) \
	list_add_first_link(&(element)->m_link, list)

#define __list_add_last(element, list, m_link) \
	list_add_last_link(&(element)->m_link, list)

#define __list_insert_before(element, list_element, m_link) \
	list_insert_before_link(&(element)->m_link, &(list_element)->m_link)

#define __list_insert_after(element, list_element, m_link) \
	list_insert_after_link(&(element)->m_link, &(list_element)->m_link)

#define __list_remove_first(list, type, m_link) \
	__list_link_element_check(list_remove_first_link(list), type, m_link)

#define __list_remove_last(list, type, m_link) \
	__list_link_element_check(list_remove_last_link(list), type, m_link)

#define __list_remove(element, m_link) \
	list_remove_link(&(element)->m_link)

inline static struct list_link *list_link_init(struct list_link *link) {
	assert(link != NULL);
	link->next = link;
	link->prev = link;
	return link;
}

inline static struct list *list_init(struct list *list) {
	assert(list != NULL);
	list_link_init(&list->link);
	return list;
}

inline static int list_alone_link(struct list_link *link) {
	assert(link != NULL);
	return link == link->next;
}

inline static int list_empty(struct list *list) {
	assert(list != NULL);
	return list_alone_link(&list->link);
}

inline static struct list_link *list_first_link(struct list *list) {
	struct list_link *l;

	assert(list != NULL);
	l = &list->link;
	return /* list is not empty? */l->next != l ? l->next : NULL;
}

inline static struct list_link *list_last_link(struct list *list) {
	struct list_link *l;

	assert(list != NULL);
	l = &list->link;
	return /* list is not empty? */l->prev != l ? l->prev : NULL;
}

inline static void __list_bind(struct list_link *prev, struct list_link *next) {
	next->prev = prev;
	prev->next = next;
}

inline static void __list_insert_chain(struct list_link *chain_first,
		struct list_link *chain_last, struct list_link *prev,
		struct list_link *next) {
	__list_bind(prev, chain_first);
	__list_bind(chain_last, next);
}

inline static void __list_insert_link(struct list_link *link,
		struct list_link *prev, struct list_link *next) {
	__list_insert_chain(link, link, prev, next);
}

inline static void list_insert_before_link(struct list_link *new_link,
		struct list_link *link) {
	assert(link != NULL);
	assert(!list_alone_link(link));
	assert(list_alone_link(new_link));

	__list_insert_link(new_link, link->prev, link);
}

inline static void list_insert_after_link(struct list_link *new_link,
		struct list_link *link) {
	assert(link != NULL);
	assert(!list_alone_link(link));
	assert(list_alone_link(new_link));

	__list_insert_link(new_link, link, link->next);
}

inline static void list_add_first_link(struct list_link *new_link,
		struct list *list) {
	struct list_link *l;

	assert(list != NULL);
	assert(list_alone_link(new_link));

	l = &list->link;
	__list_insert_link(new_link, l, l->next);
}

inline static void list_add_last_link(struct list_link *new_link,
		struct list *list) {
	struct list_link *l;

	assert(list != NULL);
	assert(list_alone_link(new_link));

	l = &list->link;
	__list_insert_link(new_link, l->prev, l);
}

inline static void list_bulk_add_first(struct list *from_list,
		struct list *to_list) {
	struct list_link *from, *to;

	assert(to_list != NULL);

	if (!list_empty(from_list)) {
		from = &from_list->link;
		to = &to_list->link;

		__list_insert_chain(from->next, from->prev, to, to->next);
		list_link_init(from);
	}
}

inline static void list_bulk_add_last(struct list *from_list,
		struct list *to_list) {
	struct list_link *from, *to;

	assert(to_list != NULL);

	if (!list_empty(from_list)) {
		from = &from_list->link;
		to = &to_list->link;

		__list_insert_chain(from->next, from->prev, to->prev, to);
		list_link_init(from);
	}
}

inline static void list_remove_link(struct list_link *link) {
	assert(link != NULL);

	__list_bind(link->prev, link->next);
	list_link_init(link);
}

inline static struct list_link *list_remove_first_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_first_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}

inline static struct list_link *list_remove_last_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_last_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}

#endif /* UTIL_LIST_IMPL_H_ */
