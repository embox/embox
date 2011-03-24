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

#define __list_link_element(link, type, m_link) \
		structof(link, type, m_link)

#define __LIST_INIT(list) { \
		.link = LIST_LINK_INIT(&(list)->link) \
	}

#define __LIST_LINK_INIT(link) { \
		.next = (link), \
		.prev = (link)  \
	}

inline static struct list_link *list_link_init(struct list_link *link) {
	link->next = link;
	link->prev = link;
	return link;
}

inline static struct list *list_init(struct list *list) {
	list_link_init(&list->link);
	return list;
}

inline static int list_alone_link(struct list_link *link) {
	return link == link->next;
}

#define __list_alone(element, m_link) \
	list_alone_link(&(element)->m_link)

inline static int list_empty(struct list *list) {
	return list_alone_link(&list->link);
}

inline static struct list_link *list_first_link(struct list *list) {
	struct list_link *l = &list->link;
	return l->next != l /* list is not empty */ ? l->next : NULL;
}

inline static struct list_link *list_last_link(struct list *list) {
	struct list_link *l = &list->link;
	return l->prev != l /* list is not empty */ ? l->prev : NULL;
}

#define __list_first(list, type, m_link) \
	__list_link_checked_element(list_first_link(list), type, m_link)

#define __list_last(list, type, m_link) \
	__list_link_checked_element(list_last_link(list), type, m_link)

#define __list_link_checked_element(link, type, m_link) \
	__extension__ ({ \
		struct list_link *__list_link__ = (link); \
		__list_link__ ? list_link_element(__list_link__, type, m_link) : NULL;\
	})

inline static void __list_insert_between(struct list_link *new,
		struct list_link *prev, struct list_link *next) {
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

inline static void list_add_first_link(struct list_link *new_link,
		struct list *list) {
	struct list_link *l = &list->link;
	assert(list_alone_link(new_link));
	__list_insert_between(new_link, l, l->next);
}

inline static void list_add_last_link(struct list_link *link, struct list *list) {
	struct list_link *l = &list->link;
	assert(list_alone_link(link));
	__list_insert_between(link, l->prev, l);
}

#define __list_add_first(element, list, m_link) \
	list_add_first_link(&(element)->m_link, list)

#define __list_add_last(element, list, m_link) \
	list_add_last_link(&(element)->m_link, list)

inline static void list_insert_before_link(struct list_link *new_link,
		struct list_link *link) {
	assert(list_alone_link(new_link));
	__list_insert_between(new_link, link->prev, link);
}

inline static void list_insert_after_link(struct list_link *new_link,
		struct list_link *link) {
	assert(list_alone_link(new_link));
	__list_insert_between(new_link, link, link->next);
}

#define __list_insert_before(element, list_element, m_link) \
	list_insert_before_link(&(element)->m_link, &(list_element)->m_link)

#define __list_insert_after(element, list_element, m_link) \
	list_insert_after_link(&(element)->m_link, &(list_element)->m_link)

inline static void list_remove_link(struct list_link *link) {
	struct list_link *prev = link->prev, *next = link->next;
	next->prev = prev;
	prev->next = next;
	list_link_init(link);
}

#define __list_remove(element, m_link) \
	list_remove_link(&(element)->m_link)

#endif /* UTIL_LIST_IMPL_H_ */
