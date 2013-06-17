/**
 * @file
 * @brief Release implementation of doubly-linked lists.
 *
 * @date 27.02.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_NDEBUG_H_
#define UTIL_LIST_NDEBUG_H_

#include <stddef.h>

struct list_link {
	struct list_link *next;
	struct list_link *prev;
};

struct list {
	struct list_link sentinel;
};

#define __LIST_LINK_INIT(link) \
	{ \
		.next = (link), \
		.prev = (link), \
	}

#define __LIST_INIT(list) \
	{ \
		.sentinel = LIST_LINK_INIT(&(list)->sentinel), \
	}

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

static inline int list_alone_link(const struct list_link *link) {
	return link == link->next;
}

static inline int list_is_empty(struct list *list) {
	return list_alone_link(&list->sentinel);
}

static inline struct list_link *list_first_link(struct list *list) {
	struct list_link *sentinel = &list->sentinel, *first = sentinel->next;
	return first != sentinel ? first : NULL;
}

static inline struct list_link *list_last_link(struct list *list) {
	struct list_link *sentinel = &list->sentinel, *last = sentinel->prev;
	return last != sentinel ? last : NULL;
}

static inline struct list_link * list_next_link(const struct list_link *link,
		struct list *list) {
	return link != list_last_link(list) ? link->next : NULL;
}

static inline struct list_link * list_prev_link(struct list_link *link,
		struct list *list) {
	return link != list_first_link(list) ? link->prev : NULL;
}

static inline void list_add_first_link(struct list_link *new_link,
		struct list *list) {
	struct list_link *sentinel = &list->sentinel;
	__list_bind(new_link, sentinel->next);
	__list_bind(sentinel, new_link);
}

static inline void list_add_last_link(struct list_link *new_link,
		struct list *list) {
	struct list_link *sentinel = &list->sentinel;
	__list_bind(sentinel->prev, new_link);
	__list_bind(new_link, sentinel);
}

static inline void list_insert_after_link(struct list_link *new_link,
		struct list_link *link) {
	__list_bind(new_link, link->next);
	__list_bind(link, new_link);
}

static inline void list_insert_before_link(struct list_link *new_link,
		struct list_link *link) {
	__list_bind(link->prev, new_link);
	__list_bind(new_link, link);
}

static inline void list_unlink_link(struct list_link *link) {
	__list_bind(link->prev, link->next);
	list_link_init(link);
}

static inline struct list_link *list_remove_first_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_first_link(list))) {
		list_unlink_link(ret);
	}
	return ret;
}

static inline struct list_link *list_remove_last_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_last_link(list))) {
		list_unlink_link(ret);
	}
	return ret;
}

static inline void __list_bulk_move_from(struct list *from_list,
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
	__list_bulk_move_from(from_list, to, to->next);
}

static inline void list_bulk_add_last(struct list *from_list,
		struct list *to_list) {
	struct list_link *to = &to_list->sentinel;
	__list_bulk_move_from(from_list, to->prev, to);
}

static inline void list_bulk_insert_before_link(struct list *from_list,
		struct list_link *link) {
	__list_bulk_move_from(from_list, link->prev, link);
}

static inline void list_bulk_insert_after_link(struct list *from_list,
		struct list_link *link) {
	__list_bulk_move_from(from_list, link, link->next);
}

#endif /* UTIL_LIST_NDEBUG_H_ */
