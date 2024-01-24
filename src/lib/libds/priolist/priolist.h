/**
 * @file
 * @brief Simple 2-tier priority-sorted list based on the standard dlist.
 *
 * A primary tier of a priolist is a list of nodes. It is a regular dlist
 * maintained sorted upon adding each new element.
 *
 * A secondary tier is a headless circular list build from each node that has
 * a different priority from its predecessor. This list can be viewed as
 * an index into the node list used to optimize insertion of a new element.
 *
 * Complexity:
 *   Addition (#priolist_add())   - O(P), P is number of different priorities
 *   Removal  (#priolist_del())   - O(1)
 *   Min/max  (#priolist_first()) - O(1)
 *
 * Idea is taken from Linux plist.
 *
 * @date 04.12.13
 * @author Eldar Abusalimov
 */

#ifndef UTIL_PRIOLIST_H_
#define UTIL_PRIOLIST_H_

#include <util/member.h>
#include <util/dlist.h>

struct priolist {
	struct dlist_head node_list;
};

struct priolist_link {
	struct dlist_head node_link;  /**< elements tier, regular list (sorted) */
	struct dlist_head prio_link;  /**< priority tier, headless */
	int               prio;
};

#define PRIOLIST_INIT(list) \
	{                                               \
		.node_list = DLIST_INIT((list).node_list), \
	}

#define PRIOLIST_LINK_INIT(link) \
	{                                               \
		.node_link = DLIST_INIT((link).node_link), \
		.prio_link = DLIST_INIT((link).prio_link), \
	}

static inline void priolist_init(struct priolist *list) {
	dlist_init(&list->node_list);
}

static inline void priolist_link_init(struct priolist_link *link) {
	dlist_init(&link->node_link);
	dlist_init(&link->prio_link);
}

static inline int priolist_empty(struct priolist *list) {
	return dlist_empty(&list->node_list);
}

static inline int priolist_link_alone(struct priolist_link *link) {
	return dlist_empty(&link->node_link);
}

static inline struct priolist_link *priolist_first(struct priolist *list) {
	struct dlist_head *f = dlist_first(&list->node_list);
	return mcast_out(f, struct priolist_link, node_link);
}

static inline struct priolist_link *priolist_first_or_null(struct priolist *list) {
	struct dlist_head *f = dlist_first_or_null(&list->node_list);
	return mcast_out(f, struct priolist_link, node_link);
}

extern void priolist_add(struct priolist_link *link, struct priolist *list);
extern void priolist_del(struct priolist_link *link, struct priolist *list);

#define priolist_foreach(link, list) \
	dlist_foreach_entry_safe(link, mcast_in(list, node_list), node_link)

#define priolist_foreach_entry(ptr, list, member) \
	dlist_foreach_entry_safe(ptr, mcast_in(list, node_list), member.node_link)

#endif /* UTIL_PRIOLIST_H_ */
