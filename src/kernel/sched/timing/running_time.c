/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#include <time.h>
#include <kernel/task.h>
#include <kernel/thread/types.h>

#include <kernel/sched/sched_timing.h>

void sched_timing_init(struct schedee *s) {
	/* setup thread running time */
	s->sched_timing.running_time = 0;
	s->sched_timing.last_sync = clock();
}

clock_t sched_timing_get(struct schedee *s) {
	clock_t running = s->sched_timing.running_time;

	if (sched_active(s))
		/* Add the least recent time slice (being used now). */
		running += clock() - s->sched_timing.last_sync;

	return running;
}

void sched_timing_start(struct schedee *s) {
	s->sched_timing.last_sync = clock();
}

void sched_timing_stop(struct schedee *s) {
	struct thread *t = mcast_out(s, struct thread, schedee);
	clock_t spent = clock() - s->sched_timing.last_sync;
	s->sched_timing.running_time += spent;
	task_set_clock(t->task, task_get_clock(t->task) + spent);
}
