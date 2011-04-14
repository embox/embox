/**
 * @file
 * @brief TODO documentation for list.h -- Eldar Abusalimov
 *
 * @date Feb 27, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_NDEBUG_H_
#define UTIL_LIST_NDEBUG_H_

#include <util/structof.h>

struct list;
struct list_link;
struct __list_link;

struct list {
	struct __list_link l;
};

struct list_link {
	struct __list_link l;
};

#define __LIST_INIT(list) \
	{                                        \
		.l = __LIST_LINK_INIT__(&(list)->l), \
	}

#define __LIST_LINK_INIT(link) \
	{                                        \
		.l = __LIST_LINK_INIT__(&(link)->l), \
	}

/* Most of macros are defined through a corresponding _link method. */

#define __list_link_element(link, type, m_link) \
	structof(link, type, m_link)

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

static inline struct list_link *list_link_init(struct list_link *link) {
	__list_link_init(&link->l);
	return link;
}

static inline struct list *list_init(struct list *list) {
	__list_link_init(&list->l);
	return list;
}

static inline int list_alone_link(struct list_link *link) {
	return __list_link_alone(&link->l);
}

static inline int list_empty(struct list *list) {
	return __list_link_alone(&list->l);
}

static inline struct list_link *list_first_link(struct list *list) {
	struct __list_link *l = &list->l, *first = l->next;
	return first != l ? structof(first, struct list_link, l) : NULL;
}

static inline struct list_link *list_last_link(struct list *list) {
	struct __list_link *l = &list->l, *last = l->prev;
	return last != l ? structof(last, struct list_link, l) : NULL;
}

static inline void list_insert_before_link(struct list_link *new_link,
		struct list_link *link) {
	struct __list_link *l = &link->l;
	__list_insert_link(&new_link->l, l->prev, l);
}

static inline void list_insert_after_link(struct list_link *new_link,
		struct list_link *link) {
	struct __list_link *l = &link->l;
	__list_insert_link(&new_link->l, l, l->next);
}

static inline void list_add_first_link(struct list_link *new_link,
		struct list *list) {
	struct __list_link *l = &list->l;
	__list_insert_link(&new_link->l, l, l->next);
}

static inline void list_add_last_link(struct list_link *new_link,
		struct list *list) {
	struct __list_link *l = &list->l;
	__list_insert_link(&new_link->l, l->prev, l);
}

static inline void list_bulk_add_first(struct list *from_list,
		struct list *to_list) {
	struct __list_link *from = &from_list->l, *to = &to_list->l;

	if (!list_empty(from_list)) {
		__list_insert_chain(from->next, from->prev, to, to->next);
		__list_link_init(from);
	}
}

static inline void list_bulk_add_last(struct list *from_list,
		struct list *to_list) {
	struct __list_link *from = &from_list->l, *to = &to_list->l;

	if (!list_empty(from_list)) {
		__list_insert_chain(from->next, from->prev, to->prev, to);
		__list_link_init(from);
	}
}

static inline void list_remove_link(struct list_link *link) {
	struct __list_link *l = &link->l;
	__list_bind(l->prev, l->next);
	__list_link_init(l);
}

static inline struct list_link *list_remove_first_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_first_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}

static inline struct list_link *list_remove_last_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_last_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}

#endif /* UTIL_LIST_NDEBUG_H_ */
