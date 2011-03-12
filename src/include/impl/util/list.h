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

#if 0
#define list_foreach(element, list, link_member) \
	  __list_foreach(element, list, link_member)

#define list_get_first(list, element_type, link_member) \
	  __list_get_first(list, element_type, link_member)

#define list_get_last(list, element_type, link_member) \
	  __list_get_last(list, element_type, link_member)

#define list_add_first(element, list, link_member) \
	  __list_add_first(element, list, link_member)

#define list_add_last(element, list, link_member) \
	  __list_add_last(element, list, link_member)

#define list_remove(element, link_member) \
	  __list_remove(element, link_member)

#define list_link_element(link, element_type, link_member) \
	  __list_link_element(link, element_type, link_member)

#define list_foreach_link(link, list) \
	  __list_foreach_link(link, list)

inline static void list_add_first_link(struct list_link *link, struct list *list);
inline static void list_add_last_link(struct list_link *link, struct list *list);

inline static struct list_link *list_get_first_link(struct list *list);
inline static struct list_link *list_get_last_link(struct list *list);

inline static void list_remove_link(struct list_link *link);
#endif
