/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */
#include <kernel/thread.h>
#include <kernel/task.h>
#include <hal/cpu.h>

/** Default thread affinity mask */
#define THREAD_AFFINITY_NONE         ((unsigned int)-1)

int sched_affinity_check(struct thread *t , int mask) {
	if ((t->sched_attr.affinity & mask)
			&& (task_get_affinity(t->task) & mask)) {
		return 1;
	}

	return 0;
}

void sched_affinity_init(struct thread *t) {
	t->sched_attr.affinity = THREAD_AFFINITY_NONE;
}

void sched_affinity_set(struct thread *t, int mask) {
	t->sched_attr.affinity = mask;
}

int sched_affinity_get(struct thread *t) {
	return t->sched_attr.affinity;
}
