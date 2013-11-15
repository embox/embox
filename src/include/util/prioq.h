/**
 * @file
 * @brief TODO documentation for prioq.h -- Eldar Abusalimov
 *
 * @date 15.09.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_PRIOQ_H_
#define UTIL_PRIOQ_H_

#include <assert.h>
#include <util/member.h>
#include <util/list.h>

struct prioq {
	struct list prio_list;
};

struct prioq_link {
	struct list_link prio_link;
	struct list elem_list;
	struct list_link elem_link;
};

#define prioq_element(link, element_type, link_member) \
	member_cast_out(link, element_type, link_member)

typedef int (*prioq_comparator_t)(struct prioq_link *first,
		struct prioq_link *second);

#include <module/embox/util/Prioq.h>

/**
 * Example comparator which defines an order based on elements addresses.
 * Just to show how it could be implemented.
 *
 * @param first
 * @param second
 * @return
 *   The result of address comparison of the @a first and the @a second.
 */
static inline int prioq_address_comparator(struct prioq_link *first,
		struct prioq_link *second) {
	return first - second;
}

#define PRIOQ_INIT(prioq) { \
		.prio_list = LIST_INIT(prioq->prio_list), \
	}

#define PRIOQ_LINK_INIT(link) { \
		.prio_link = LIST_LINK_INIT(link->prio_link), \
		.elem_list = LIST_INIT(link->elem_list), \
		.elem_link = LIST_LINK_INIT(link->elem_link), \
	}

static inline struct prioq *prioq_init(struct prioq *prioq) {
	assert(prioq != NULL);

	list_init(&prioq->prio_list);

	return prioq;
}

static inline struct prioq_link *prioq_link_init(struct prioq_link *link) {
	assert(link != NULL);

	list_link_init(&link->prio_link);
	list_init(&link->elem_list);
	list_link_init(&link->elem_link);

	return link;
}

static inline int prioq_empty(struct prioq *prioq) {
	assert(prioq != NULL);

	return list_is_empty(&prioq->prio_list);
}

#define prioq_enqueue(element, link_comparator, prioq, link_member) \
	prioq_enqueue_link(&__prioq_check(element)->link_member, \
			link_comparator, prioq)

extern void prioq_enqueue_link(struct prioq_link *new_link,
		prioq_comparator_t link_comparator, struct prioq *prioq);

#define prioq_peek(link_comparator, prioq, element_type, link_member) \
	__prioq_link_safe_cast(prioq_peek_link(link_comparator, prioq), \
			element_type, link_member)

static inline struct prioq_link *prioq_peek_link(
		prioq_comparator_t link_comparator, struct prioq *prioq) {
	(void)link_comparator;
	assert(prioq != NULL);

	return list_first(__prioq_chain, &prioq->prio_list);
}

#define prioq_remove(element, link_comparator, link_member) \
	prioq_remove_link(&__prioq_check(element)->link_member, link_comparator)

extern void prioq_remove_link(struct prioq_link *link,
		prioq_comparator_t link_comparator);

#define prioq_dequeue(link_comparator, prioq, element_type, link_member) \
	__prioq_link_safe_cast(prioq_dequeue_link(link_comparator, prioq), \
			element_type, link_member)

static inline struct prioq_link *prioq_dequeue_link(
		prioq_comparator_t link_comparator, struct prioq *prioq) {
	struct prioq_link *l;

	assert(link_comparator && prioq);

	if ((l = prioq_peek_link(link_comparator, prioq))) {
		prioq_remove_link(l, link_comparator);
	}

	return l;
}

#endif /* UTIL_PRIOQ_H_ */
