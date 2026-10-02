/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <lib/libds/dlist.h>

#include <kernel/task.h>
#include <kernel/sched/sched_strategy.h>

#include <module/embox/arch/smp.h>
#include <kernel/cpu/cpu.h>

struct schedee;

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

void runq_insert(runq_t *queue, struct schedee *schedee) {
	dlist_add_prev(&schedee->runq_link,
			&queue->list[schedee_priority_get(schedee)]);
}

void runq_remove(runq_t *queue, struct schedee *schedee) {
	dlist_del(&schedee->runq_link);
}

struct schedee *runq_get_next(runq_t *queue) {
	const unsigned int mask = 1 << cpu_get_id();
	struct schedee *schedee = NULL;
	int i;

	for (i = SCHED_PRIORITY_MAX; i >= SCHED_PRIORITY_MIN; i--) {
		struct schedee *s;

		dlist_foreach_entry(s, &queue->list[i], runq_link) {
			/* Checking the affinity */

			if (sched_affinity_check(&s->affinity, mask)) {
				schedee = s;
				break;
			}
		}

		if (schedee) {
			break;
		}
	}

	return schedee;
}

struct schedee *runq_extract(runq_t *queue) {
	const unsigned int mask = 1 << cpu_get_id();
	struct schedee *schedee = NULL;
	int i;

	for (i = SCHED_PRIORITY_MAX; i >= SCHED_PRIORITY_MIN; i--) {
		struct schedee *s;

		dlist_foreach_entry(s, &queue->list[i], runq_link) {
			/* Checking the affinity */

			if (sched_affinity_check(&s->affinity, mask)) {
				schedee = s;
				break;
			}
		}

		if (schedee) {
			runq_remove(queue, schedee);
			break;
		}
	}

	return schedee;
}

struct schedee *runq_get_next_ignore_affinity(runq_t *queue) {
	struct schedee *schedee = NULL;
	int i;

	for (i = SCHED_PRIORITY_MAX; i >= SCHED_PRIORITY_MIN; i--) {
		struct schedee *s;

		dlist_foreach_entry(s, &queue->list[i], runq_link) {
			/* Not checking the affinity */
			schedee = s;
			break;
		}

		if (schedee) {
			break;
		}
	}

	return schedee;
}
