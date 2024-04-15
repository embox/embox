/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#include <lib/libds/dlist.h>
#include <kernel/thread.h>
#include <kernel/sched/waitq.h>
#include <mem/sysmalloc.h>
#include <kernel/panic.h>

#include <kernel/sched/waitq_protect_link.h>

struct waitq_link_protect {
	struct dlist_head thread_link;

	struct waitq_link *uwql;
	struct waitq_link pwql;
};

static inline struct dlist_head *thread_wql_protected_list(void) {
	return &thread_self()->waitq_list.waitq_protect_list;
}

struct waitq_link *waitq_link_create_protected(struct waitq_link *uwql) {
	struct dlist_head *wqlp_list;
	struct waitq_link_protect *wqlp;

	wqlp = sysmalloc(sizeof(*wqlp));
	assert(wqlp != NULL);

	wqlp_list = thread_wql_protected_list();

	dlist_head_init(&wqlp->thread_link);
	dlist_add_next(&wqlp->thread_link, wqlp_list);

	wqlp->uwql = uwql;

	waitq_link_init(&wqlp->pwql);

	return &wqlp->pwql;
}

struct waitq_link *waitq_link_find_protected(struct waitq_link *uwql) {
	struct dlist_head *wqlp_list;
	struct waitq_link_protect *wqlp;

	wqlp_list = thread_wql_protected_list();

	dlist_foreach_entry(wqlp, wqlp_list, thread_link) {
		if (wqlp->uwql == uwql) {
			return &wqlp->pwql;
		}
	}

	panic("%s: can't find waitq_link", __func__);

	return NULL;
}

void waitq_link_delete_protected(struct waitq_link *pwql) {
	struct waitq_link_protect *wqlp = member_cast_out(pwql, struct waitq_link_protect, pwql);

	dlist_del(&wqlp->thread_link);

	sysfree(wqlp);
}
