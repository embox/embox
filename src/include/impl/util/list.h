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

#define __list_link_element(link, element_type, link_member) \
		structof(link, element_type, link_member)

#define __LIST_INIT(list) { \
		.link = LIST_LINK_INIT(&(list)->link) \
	}

#define __LIST_LINK_INIT(link) { \
		.next = (link), \
		.prev = (link)  \
	}

inline static struct list *list_init(struct list *list) {
	return list;
}

inline static struct list_link *list_link_init(struct list_link *link) {
	return link;
}

inline static int list_empty(struct list *list) {
	return &list->link == list->link.next;
}

