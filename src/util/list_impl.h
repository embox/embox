/**
 * @file
 * @brief TODO documentation for list.h -- Eldar Abusalimov
 *
 * @date Feb 27, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_IMPL_H_
#define UTIL_LIST_IMPL_H_

struct __list_link {
	struct __list_link *next, *prev;
};

#define __LIST_LINK_INIT__(__link) \
	{                     \
		.next = (__link), \
		.prev = (__link), \
	}

inline static void __list_bind(struct __list_link *prev,
		struct __list_link *next) {
	next->prev = prev;
	prev->next = next;
}

inline static int __list_link_alone(struct __list_link *link) {
	return link == link->next;
}

inline static void __list_link_init(struct __list_link *link) {
	__list_bind(link, link);
}

inline static void __list_insert_chain(struct __list_link *first,
		struct __list_link *last, struct __list_link *prev,
		struct __list_link *next) {
	__list_bind(prev, first);
	__list_bind(last, next);
}

inline static void __list_insert_link(struct __list_link *link,
		struct __list_link *prev, struct __list_link *next) {
	__list_insert_chain(link, link, prev, next);
}

#ifdef LIST_NDEBUG
# include "list_ndebug.h"
#else
# include "list_debug.h"
#endif

#endif /* UTIL_LIST_IMPL_H_ */
