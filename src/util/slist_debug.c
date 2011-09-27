/**
 * @file
 * @brief TODO
 *
 * @date 17.04.11
 * @author Eldar Abusalimov
 */

#ifdef CONFIG_SLIST_NDEBUG
# error "Must compile this file with SLIST_NDEBUG disabled"
#endif

#include <assert.h>

#include <util/slist.h>
#include <util/member.h>

struct slist_link *slist_link_init(struct slist_link *link) {
	assert(link != NULL);

	link->poison = __SLIST_LINK_POISON;
	link->slist = NULL;
	__slist_link_init(&link->l);

	return link;
}

struct slist *slist_init(struct slist *list) {
	assert(list != NULL);

	list->poison = __SLIST_POISON;
	list->offset = -1;
	__slist_link_init(&list->l);

	return list;
}

int slist_alone_link(struct slist_link *link) {
	assert(link != NULL);
	return __slist_link_alone(&link->l);
}

int slist_empty(struct slist *list) {
	assert(list != NULL);
	return __slist_link_alone(&list->l);
}

struct slist_link *slist_first_link(struct slist *list) {
	struct __slist_link *l, *first;

	assert(list != NULL);
	l = &list->l;
	first = l->next;

	assert(first != NULL);
	return first != l ? member_cast_out(first, struct slist_link, l) : NULL;
}

void slist_add_first_link(struct slist_link *new_link, struct slist *list) {
	struct __slist_link *l;

	assert(list != NULL);
	assert(slist_alone_link(new_link));

	l = &list->l;
	__slist_insert_link(&new_link->l, l, l->next);
}

void slist_insert_after_link(struct slist_link *new_link, struct slist_link *link) {
	struct __slist_link *l;

	assert(link != NULL);
	assert(!slist_alone_link(link));
	assert(slist_alone_link(new_link));

	l = &link->l;
	__slist_insert_link(&new_link->l, l, l->next);
}

struct slist_link *slist_remove_first_link(struct slist *list) {
	struct slist_link *ret;
	struct __slist_link *l, *first;

	if ((ret = slist_first_link(list))) {
		first = &ret->l;
		l = &list->l;

		__slist_bind(l, first->next);
		__slist_link_init(first);
	}

	return ret;
}
