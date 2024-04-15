/**
 * @file
 * @brief 2-tier priority-sorted list.
 *
 * @date 04.12.13
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <stddef.h>

#include <lib/libds/priolist.h>

void priolist_add(struct priolist_link *link, struct priolist *list) {
	struct dlist_head *next_node_link = &list->node_list;

	assert(link && list);
	assert(dlist_empty(&link->node_link));
	assert(dlist_empty(&link->prio_link));

	if (!priolist_empty(list)) {
		struct priolist_link *first = priolist_first(list);
		struct priolist_link *iter = first, *last = NULL;

		do {
			if (iter->prio > link->prio) {
				next_node_link = &iter->node_link;
				break;
			}

			last = iter;
			iter = mcast_out(iter->prio_link.next,
					struct priolist_link, prio_link);
		} while (iter != first);

		if (!last || last->prio != link->prio)
			/* new prio, new element in a prio tier */
			dlist_add_prev(&link->prio_link, &iter->prio_link);
	}

	dlist_add_prev(&link->node_link, next_node_link);
}

void priolist_del(struct priolist_link *link, struct priolist *list) {
	assert(link && list);
	assert(!dlist_empty(&link->node_link));
	assert(!dlist_empty(&list->node_list));

	if (!dlist_empty(&link->prio_link)) {
		struct dlist_head *next_node_link = link->node_link.next;

		if (next_node_link != &list->node_list) {
			struct priolist_link *next =
				mcast_out(next_node_link,
						struct priolist_link,
						node_link);

			if (dlist_empty(&next->prio_link))
				dlist_add_next(&next->prio_link, &link->prio_link);
		}

		dlist_del_init(&link->prio_link);
	}

	dlist_del_init(&link->node_link);
}

