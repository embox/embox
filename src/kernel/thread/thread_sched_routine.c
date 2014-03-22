/*
	Thread planning functions, used in sched_switch

	Implements runnable.h interface
*/

#include <hal/context.h>
#include <hal/ipl.h>
#include <stdlib.h>
#include <assert.h>
#include <sched.h>
#include <kernel/sched.h>
#include <kernel/critical.h>
#include <kernel/thread.h>
#include <kernel/runnable/runnable.h>
#include <kernel/thread/state.h>
#include <profiler/tracing/trace.h>

void sched_prepare_thread(struct thread *prev, struct runnable *n) {
	struct thread *next;

	next = mcast_out(n, struct thread, runnable);
	sched_thread_switch(prev, next);
}

struct runnable *runnable_get_current(){
	return &(thread_get_current()->runnable);
}
