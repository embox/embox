/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#include <time.h>
#include <kernel/thread.h>
#include <kernel/thread/state.h>
#include <kernel/task.h>

#include <kernel/sched/sched_timing.h>

void sched_timing_init(struct thread *t) {
	/* setup thread running time */
	t->sched_attr.sched_time.running_time = 0;
	t->sched_attr.sched_time.last_sync = clock();
}

clock_t sched_timing_get(struct thread *t) {
	clock_t running;

	/* if thread is executing now we have to add recent CPU time slice. */
	if (thread_state_oncpu(t->state)) {
		running = clock() - t->sched_attr.sched_time.last_sync;
		running += t->sched_attr.sched_time.running_time;
	} else {
		running = t->sched_attr.sched_time.running_time;
	}

	return running;
}

void sched_timing_stop(struct thread *t, clock_t cur_time) {
	clock_t spent;

	spent = cur_time - t->sched_attr.sched_time.last_sync;

	t->sched_attr.sched_time.running_time += spent;
	//TODO this is for qt
	t->task->per_cpu += spent;
}

void sched_timing_start(struct thread *t, clock_t cur_time) {
	t->sched_attr.sched_time.last_sync = cur_time;
}
