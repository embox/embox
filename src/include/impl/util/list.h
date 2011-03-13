/**
 * @file
 * @brief TODO documentation for list.h -- Eldar Abusalimov
 *
 * @date Feb 27, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_H_
# error "Do not include this file directly, use <util/list.h> instead!"
#endif /* UTIL_LIST_H_ */

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

inline static int list_empty(struct list *list) {
	struct list_link *link = &list->link;
	return link == link->next;
}

inline static struct list_link *list_first_link(struct list *list) {
	struct list_link *link = &list->link;
	return link->next != link /* list is not empty */ ? link->next : NULL;
}

inline static struct list_link *list_last_link(struct list *list) {
	struct list_link *link = &list->link;
	return link->prev != link /* list is not empty */ ? link->prev : NULL;
}

inline static void list_add_first_link(struct list_link *link,
		struct list *list) {
	;
}
inline static void list_add_last_link(struct list_link *link, struct list *list) {
	;
}

#define __list_link_checked_element(link, type, link_member) \
	__extension__ ({ \
		struct list_link *__list_link__ = (link); \
		__list_link__ \
				? list_link_element(__list_link__, type, link_member) \
				: NULL; \
	})

#define __list_first(list, type, link_member) \
		__list_link_checked_element(list_first_link(list), type, link_member)

#define __list_last(list, type, link_member) \
		__list_link_checked_element(list_last_link(list), type, link_member)

#define __list_add_first(element, list, link_member) \
		list_add_first_link(&(element)->link_member, list)

#define __list_add_last(element, list, link_member) \
		list_add_last_link(&(element)->link_member, list)

