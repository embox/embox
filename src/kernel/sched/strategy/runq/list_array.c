/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>

#include <kernel/runnable/runnable.h>
#include <kernel/task.h>
#include <kernel/sched/sched_strategy.h>

#include <module/embox/arch/smp.h>
#include <kernel/cpu/cpu.h>


void runq_item_init(runq_item_t *runq_link) {
	dlist_head_init(runq_link);
}

/* runq operations */

void runq_init(runq_t *queue) {
	int i;

	for (i = SCHED_PRIORITY_MIN; i <= SCHED_PRIORITY_MAX; i++) {
		dlist_init(&queue->list[i]);
	}
}

void runq_insert(runq_t *queue, struct runnable *runnable) {
	dlist_add_prev(&runnable->sched_attr.runq_link,
			&queue->list[runnable_priority_get(runnable)]);
}

void runq_remove(runq_t *queue, struct runnable *runnable) {
	dlist_del(&runnable->sched_attr.runq_link);
}

struct runnable *runq_extract(runq_t *queue) {
	struct runnable *runnable = NULL;
	int i;

	for (i = SCHED_PRIORITY_MAX; i >= SCHED_PRIORITY_MIN; i--) {
		struct runnable *r;

		dlist_foreach_entry(r, &queue->list[i], sched_attr.runq_link) {
			/* Checking the affinity */
			unsigned int mask = 1 << cpu_get_id();

			if (sched_affinity_check(r, mask)) {
				runnable = r;
				break;
			}
		}

		if (runnable) {
			runq_remove(queue, runnable);
			break;
		}
	}

	return runnable;
}
