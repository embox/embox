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

#define __list_link_element(link, type, link_member) \
		structof(link, type, link_member)

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

#define __list_alone(element, link_member) \
	list_alone_link(&(element)->link_member)

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

#define __list_first(list, type, link_member) \
	__list_link_checked_element(list_first_link(list), type, link_member)

#define __list_last(list, type, link_member) \
	__list_link_checked_element(list_last_link(list), type, link_member)

#define __list_link_checked_element(link, type, link_member) \
	__extension__ ({ \
		struct list_link *__list_link__ = (link); \
		__list_link__ \
				? list_link_element(__list_link__, type, link_member) \
				: NULL; \
	})

inline static void __list_insert_between(struct list_link *new,
		struct list_link *prev, struct list_link *next) {
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

inline static void list_add_first_link(struct list_link *link,
		struct list *list) {
	struct list_link *l = &list->link;
	assert(list_alone_link(link));
	__list_insert_between(link, l, l->next);
}

inline static void list_add_last_link(struct list_link *link, struct list *list) {
	struct list_link *l = &list->link;
	assert(list_alone_link(link));
	__list_insert_between(link, l->prev, l);
}

#define __list_add_first(element, list, link_member) \
	list_add_first_link(&(element)->link_member, list)

#define __list_add_last(element, list, link_member) \
	list_add_last_link(&(element)->link_member, list)

inline static void list_remove_link(struct list_link *link) {
	struct list_link *prev = link->prev, *next = link->next;
	next->prev = prev;
	prev->next = next;
	list_link_init(link);
}

#define __list_remove(element, link_member) \
	list_remove_link(&(element)->link_member)

#endif /* UTIL_LIST_IMPL_H_ */
