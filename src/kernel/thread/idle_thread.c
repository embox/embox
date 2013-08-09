/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */

#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_alloc.h>
#include <hal/arch.h> /*only for arch_idle */


#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

/*
 * Function, which does nothing. For idle_thread.
 */
static void *idle_run(void *arg) {
	while (true) {
		arch_idle();
	}
	return NULL;
}


struct thread *idle_thread_create(void) {
	struct thread *t;
	sched_priority_t prior;

	if (!(t = thread_alloc())) {
		return NULL;
	}


	thread_init(t, THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED, idle_run, NULL);
	t->task = task_kernel_task();
	t->task->main_thread = t;

	prior = sched_priority_full(t->task->priority, THREAD_PRIORITY_MIN);

	thread_priority_set(t, prior);

	cpu_init(cpu_get_id(), t);

	return t;
}
