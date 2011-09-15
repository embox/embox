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
	__extension__ ({                        \
		typeof(expr) __prioq_expr = (expr); \
		assert(__prioq_expr != NULL);       \
		__prioq_expr;                       \
	})

#define __prioq_link_safe_cast(link, type, m_link) \
	__extension__ ({ \
		struct prioq_link *__prioq_link__ = (link); \
		__prioq_link__ \
				? prioq_link_element(__prioq_link__, type, m_link) \
				: NULL; \
	})

static inline void prioq_enqueue_link(struct prioq_link *new_link,
		prioq_comparator_t link_comparator, struct prioq *prioq) {
	struct prioq_link *next, *found = NULL;
	int comparison = 0;

	assert(new_link && link_comparator && prioq);

	list_for_each_entry(next, &prioq->prio_list, prio_link) {
		if ((comparison = link_comparator(next, new_link)) <= 0) {
			found = next;
			break;
		}
	}

	// TODO May be extract the following into a non-inline function. -- Eldar
	if (!found) {
		/* All of the existing elements (if any) have higher priority. */
		list_add_tail(&new_link->prio_link, &prioq->prio_list);

	} else if (comparison <= 0) {
		/* Found a chain with lower priority. */
		list_add(&new_link->prio_link, &found->prio_link);

	} else {
		/* Found a chain with the same priority. */
		list_add_tail(&new_link->elem_link, &found->elem_link);

	}
}

#endif /* UTIL_PRIOQ_IMPL_H_ */
