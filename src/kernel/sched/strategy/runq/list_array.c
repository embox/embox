/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>

#include <kernel/thread.h>
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

void runq_insert(runq_t *queue, struct thread *thread) {
	dlist_add_prev(&thread->sched_attr.runq_link,
			&queue->list[thread_priority_get(thread)]);
}

void runq_remove(runq_t *queue, struct thread *thread) {
	dlist_del(&thread->sched_attr.runq_link);
}

struct thread *runq_extract(runq_t *queue) {
	struct thread *thread = NULL;
	int i;

	for (i = SCHED_PRIORITY_MAX; i >= SCHED_PRIORITY_MIN; i--) {
		struct thread *t;
		dlist_foreach_entry(t, &queue->list[i], sched_attr.runq_link) {
			/* Checking the affinity */
			unsigned int mask = 1 << cpu_get_id();

			if (sched_affinity_check(t, mask)) {
				thread = t;
				break;
			}
		}

		if (thread) {
			runq_remove(queue, thread);
			break;
		}
	}

	return thread;
}
