/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>
#include <util/prioq.h>

#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched/sched_strategy.h>

#include <module/embox/arch/smp.h>
#include <kernel/cpu/cpu.h>

void sched_strategy_init(struct sched_strategy_data *s) {
	dlist_head_init(&s->link);
	prioq_link_init(&s->pq_link);
}

/* runq operations */

void runq_queue_init(runq_queue_t *queue) {
	int i;

	for (i = SCHED_PRIORITY_MIN; i <= SCHED_PRIORITY_MAX; i++) {
		dlist_init(&queue->list[i]);
	}
}

void runq_queue_insert(runq_queue_t *queue, struct thread *thread) {
	dlist_add_prev(&thread->sched_priv.link,
			&queue->list[thread->thread_priority.sched_priority]);
}

void runq_queue_remove(runq_queue_t *queue, struct thread *thread) {
	dlist_del(&thread->sched_priv.link);
}

struct thread *runq_queue_extract(runq_queue_t *queue) {
	struct thread *thread = NULL;
	int i;

	for (i = SCHED_PRIORITY_MAX; i >= SCHED_PRIORITY_MIN; i--) {
#ifdef NOSMP
		if (!dlist_empty(&queue->list[i])) {
			thread = dlist_entry(queue->list[i].next, struct thread,
					sched_priv.link);
		}
#else /* NOSMP */
		struct thread *t, *nxt;
		unsigned int mask = 1 << cpu_get_id();
		dlist_foreach_entry(t, nxt, &queue->list[i], sched_priv.link) {
			/* Checking the affinity */
			if ((t->affinity & mask)
				&& (task_get_affinity(t->task) & mask)) {
				thread = t;
				break;
			}
		}
#endif

		if (thread) {
			runq_queue_remove(queue, thread);
			break;
		}
	}

	return thread;
}
