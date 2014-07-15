/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#include <time.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched.h>

#include <kernel/sched/sched_timing.h>

void sched_timing_init(struct thread *t) {
	/* setup thread running time */
	t->runnable.sched_attr.sched_time.running_time = 0;
	t->runnable.sched_attr.sched_time.last_sync = clock();
}

clock_t sched_timing_get(struct thread *t) {
	clock_t running = t->runnable.sched_attr.sched_time.running_time;

	if (sched_active(t))
		/* Add the least recent time slice (being used now). */
		running += clock() - t->runnable.sched_attr.sched_time.last_sync;

	return running;
}

static void sched_timing_start(struct thread *t, clock_t cur_time) {
	t->runnable.sched_attr.sched_time.last_sync = cur_time;
}

static void sched_timing_stop(struct thread *t, clock_t cur_time) {
	clock_t spent = cur_time - t->runnable.sched_attr.sched_time.last_sync;

	t->runnable.sched_attr.sched_time.running_time += spent;
	//TODO this is for qt
	task_set_clock(t->task, task_get_clock(t->task) + spent);
}

void sched_timing_switch(struct thread *prev, struct thread *next) {
	clock_t new_clock;

	/* Running time recalculation */
	new_clock = clock();
	sched_timing_stop(prev, new_clock);
	sched_timing_start(next, new_clock);
}
