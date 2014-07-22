/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */
#include <kernel/runnable/runnable.h>

#include <kernel/task.h>
#include <kernel/task/resource/affinity.h>
#include <hal/cpu.h>

/** Default runnable affinity mask */
#define RUNNABLE_AFFINITY_NONE         ((unsigned int)-1)

int sched_affinity_check(struct runnable *r , int mask) {
	if ((r->sched_attr.affinity & mask)){
			/*&& (task_get_affinity(t->task) & mask)) {*/
			/*< TODO: runnable hasn't got task inside */
		return 1;
	}

	return 0;
}

void sched_affinity_init(struct runnable *r) {
	r->sched_attr.affinity = RUNNABLE_AFFINITY_NONE;
}

void sched_affinity_set(struct runnable *r, int mask) {
	r->sched_attr.affinity = mask;
}

int sched_affinity_get(struct runnable *r) {
	return r->sched_attr.affinity;
}
