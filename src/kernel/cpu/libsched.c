/**
 * @file
 *
 * @date Jul 19, 2013
 * @author: Anton Bondarev
 */

#include <time.h>
#include <kernel/thread.h>

extern struct runq rq;
extern int runq_cpu_init(struct runq *rq, struct thread *current);

int sched_cpu_init(struct thread *current) {
	runq_cpu_init(&rq, current);

	current->last_sync = clock();
	thread_set_current(current);

	return 0;
}
