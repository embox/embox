/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */
#include <kernel/sched/schedee.h>

#include <kernel/task.h>
#include <kernel/task/resource/affinity.h>
#include <hal/cpu.h>

/** Default schedee affinity mask */
#define SCHEDEE_AFFINITY_NONE         ((unsigned int)-1)

int sched_affinity_check(struct schedee *s, int mask) {
	if ((s->affinity & mask)){
			/*&& (task_get_affinity(t->task) & mask)) {*/
			/*< TODO: schedee hasn't got task inside */
		return 1;
	}

	return 0;
}

void sched_affinity_init(struct schedee *s) {
	s->affinity = SCHEDEE_AFFINITY_NONE;
}

void sched_affinity_set(struct schedee *s, int mask) {
	s->affinity = mask;
}

int sched_affinity_get(struct schedee *s) {
	return s->affinity;
}
