#ifndef DLIST_PRIO_H_
#define DLIST_PRIO_H_

#include <lib/libds/dlist.h>
#include <kernel/sched/schedee_priority.h>

struct runq_queue {
	struct dlist_head list[SCHED_PRIORITY_TOTAL];
};

typedef struct dlist_head runq_item_t;

typedef struct runq_queue runq_t;

#define __RUNQ_ITEM_INIT(item) \
	DLIST_INIT(item)

#endif /* DLIST_PRIO_H */