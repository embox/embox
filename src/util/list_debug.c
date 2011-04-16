/**
 * @file
 * @brief TODO
 *
 * @date Mar 26, 2011
 * @author Eldar Abusalimov
 */

#ifdef CONFIG_LIST_NDEBUG
# error "Must compile this file with LIST_NDEBUG disabled"
#endif

#include <assert.h>

#include <util/list.h>
#include <util/structof.h>

struct list_link *list_link_init(struct list_link *link) {
	assert(link != NULL);

	link->poison = __LIST_LINK_POISON;
	link->list = NULL;
	__list_link_init(&link->l);

	return link;
}

struct list *list_init(struct list *list) {
	assert(list != NULL);

	list->poison = __LIST_POISON;
	list->offset = -1;
	__list_link_init(&list->l);

	return list;
}

int list_alone_link(struct list_link *link) {
	assert(link != NULL);
	return __list_link_alone(&link->l);
}

int list_empty(struct list *list) {
	assert(list != NULL);
	return __list_link_alone(&list->l);
}

struct list_link *list_first_link(struct list *list) {
	struct __list_link *l, *first;

	assert(list != NULL);
	l = &list->l;
	first = l->next;
	return first != l ? structof(first, struct list_link, l) : NULL;
}

struct list_link *list_last_link(struct list *list) {
	struct __list_link *l, *last;

	assert(list != NULL);
	l = &list->l;
	last = l->prev;
	return last != l ? structof(last, struct list_link, l) : NULL;
}

void list_add_first_link(struct list_link *new_link, struct list *list) {
	struct __list_link *l;

	assert(list != NULL);
	assert(list_alone_link(new_link));

	l = &list->l;
	__list_insert_link(&new_link->l, l, l->next);
}

void list_add_last_link(struct list_link *new_link, struct list *list) {
	struct __list_link *l;

	assert(list != NULL);
	assert(list_alone_link(new_link));

	l = &list->l;
	__list_insert_link(&new_link->l, l->prev, l);
}

void list_insert_before_link(struct list_link *new_link, struct list_link *link) {
	struct __list_link *l;

	assert(link != NULL);
	assert(!list_alone_link(link));
	assert(list_alone_link(new_link));

	l = &link->l;
	__list_insert_link(&new_link->l, l->prev, l);
}

void list_insert_after_link(struct list_link *new_link, struct list_link *link) {
	struct __list_link *l;

	assert(link != NULL);
	assert(!list_alone_link(link));
	assert(list_alone_link(new_link));

	l = &link->l;
	__list_insert_link(&new_link->l, l, l->next);
}

void list_bulk_add_first(struct list *from_list, struct list *to_list) {
	struct __list_link *from, *to;

	assert(to_list != NULL);

	if (!list_empty(from_list)) {
		from = &from_list->l;
		to = &to_list->l;

		__list_insert_chain(from->next, from->prev, to, to->next);
		__list_link_init(from);
	}
}

void list_bulk_add_last(struct list *from_list, struct list *to_list) {
	struct __list_link *from, *to;

	assert(to_list != NULL);

	if (!list_empty(from_list)) {
		from = &from_list->l;
		to = &to_list->l;

		__list_insert_chain(from->next, from->prev, to->prev, to);
		__list_link_init(from);
	}
}

void list_bulk_insert_before_link(struct list *from_list,
		struct list_link *link) {
	struct __list_link *from, *l;

	assert(link != NULL);

	if (!list_empty(from_list)) {
		from = &from_list->l;
		l = &link->l;

		__list_insert_chain(from->next, from->prev, l->prev, l);
		__list_link_init(from);
	}
}

void list_bulk_insert_after_link(struct list *from_list,
		struct list_link *link) {
	struct __list_link *from, *l;

	assert(link != NULL);

	if (!list_empty(from_list)) {
		from = &from_list->l;
		l = &link->l;

		__list_insert_chain(from->next, from->prev, l, l->next);
		__list_link_init(from);
	}
}

void list_remove_link(struct list_link *link) {
	struct __list_link *l;

	assert(link != NULL);

	l = &link->l;
	__list_bind(l->prev, l->next);
	__list_link_init(l);
}

struct list_link *list_remove_first_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_first_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}

struct list_link *list_remove_last_link(struct list *list) {
	struct list_link *ret;

	if ((ret = list_last_link(list))) {
		list_remove_link(ret);
	}

	return ret;
}
