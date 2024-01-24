/**
 * @file
 * @brief Release implementation of single linked lists.
 *
 * @date 17.04.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_SLIST_NDEBUG_H_
#define UTIL_SLIST_NDEBUG_H_

#include <util/member.h>

struct slist;
struct slist_link;
struct __slist_link;

struct slist {
	struct __slist_link l;
};

struct slist_link {
	struct __slist_link l;
};

#define __SLIST_INIT(slist) \
	{                                        \
		.l = __SLIST_LINK_INIT__(&(slist)->l), \
	}

#define __SLIST_LINK_INIT(link) \
	{                                        \
		.l = __SLIST_LINK_INIT__(&(link)->l), \
	}

static inline struct slist_link *slist_link_init(struct slist_link *link) {
	__slist_link_init(&link->l);
	return link;
}

static inline struct slist *slist_init(struct slist *list) {
	__slist_link_init(&list->l);
	return list;
}

static inline int slist_alone_link(struct slist_link *link) {
	return __slist_link_alone(&link->l);
}

static inline int slist_empty(struct slist *list) {
	return __slist_link_alone(&list->l);
}

static inline struct slist_link *slist_first_link(struct slist *list) {
	struct __slist_link *l = &list->l, *first = l->next;
	return first != l ? member_cast_out(first, struct slist_link, l) : NULL;
}

static inline void slist_add_first_link(struct slist_link *new_link,
		struct slist *list) {
	struct __slist_link *l = &list->l;
	__slist_insert_link(&new_link->l, l, l->next);
}

static inline void slist_insert_after_link(struct slist_link *new_link,
		struct slist_link *link) {
	struct __slist_link *l = &link->l;
	__slist_insert_link(&new_link->l, l, l->next);
}

static inline struct slist_link *slist_remove_first_link(struct slist *list) {
	struct slist_link *ret;
	struct __slist_link *l = &list->l, *first;

	if ((ret = slist_first_link(list))) {
		first = &ret->l;

		__slist_bind(l, first->next);
		__slist_link_init(first);
	}

	return ret;
}

#endif /* UTIL_SLIST_NDEBUG_H_ */
