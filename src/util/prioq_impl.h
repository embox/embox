/**
 * @file
 * @brief TODO documentation for prioq_impl.h -- Eldar Abusalimov
 *
 * @date Sep 15, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_PRIOQ_IMPL_H_
#define UTIL_PRIOQ_IMPL_H_

#define __prioq_check(expr) \
	({ \
		typeof(expr) __prioq_expr = (expr); \
		assert(__prioq_expr != NULL);       \
		__prioq_expr;                       \
	})

#define __prioq_link_safe_cast(link, type, m_link) \
	({ \
		struct prioq_link *__prioq_link__ = (link); \
		__prioq_link__ \
				? prioq_element(__prioq_link__, type, m_link) \
				: NULL; \
	})

typedef member_t(struct prioq_link, prio_link) __prioq_chain;

static inline void prioq_enqueue_link(struct prioq_link *new_link,
		prioq_comparator_t link_comparator, struct prioq *prioq) {
	struct prioq_link *next, *found = NULL;
	int comparison = 0;

	assert(new_link && link_comparator && prioq);

	list_foreach(next, &prioq->prio_list, prio_link) {
		if ((comparison = link_comparator(next, new_link)) <= 0) {
			found = next;
			break;
		}
	}

	// TODO May be extract the following into a non-inline function. -- Eldar
	// TODO Don't like it, too many code repetition
	if (!found) {
		/* All of the existing elements (if any) have higher priority. */
		list_add_last(__prioq_chain, new_link, &prioq->prio_list);
		assert(list_is_empty(&new_link->elem_list));
		assert(list_alone_link(&new_link->elem_link));

	} else if (comparison < 0) {
		/* Found a chain with lower priority. */
		list_insert_before(__prioq_chain, new_link, found);
		assert(list_is_empty(&new_link->elem_list));
		assert(list_alone_link(&new_link->elem_link));

	} else {
		/* Found a chain with the same priority. */
		list_add_last_link(&new_link->elem_link, &found->elem_list);
		assert(!list_is_empty(&found->elem_list));
		assert(list_is_empty(&new_link->elem_list));
		assert(!list_alone_link(&new_link->elem_link));

	}
}

static inline void prioq_remove_link(struct prioq_link *link,
		prioq_comparator_t link_comparator) {
	assert(link && link_comparator);

	if (list_alone(__prioq_chain, link)) {
		assert(list_is_empty(&link->elem_list));
		assert(!list_alone_link(&link->elem_link));
		list_unlink_link(&link->elem_link);
		return;
	}

	if (!list_is_empty(&link->elem_list)) {
		struct prioq_link *new_link = list_first_element(&link->elem_list,
				struct prioq_link, elem_link);

		/* Replace priority link being deleted with a new one. */
		list_insert_after(__prioq_chain, new_link, link);

		list_unlink_link(&new_link->elem_link);
		list_bulk_add_first(&link->elem_list, &new_link->elem_list);
	}

	list_unlink_link(&link->prio_link);
}

#endif /* UTIL_PRIOQ_IMPL_H_ */
