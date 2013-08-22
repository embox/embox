/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */

#include <kernel/task.h>
#include <kernel/thread.h>
#include <err.h>
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

	t = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED, idle_run,
			NULL);

	if(err(t)) {
		return NULL;
	}

	t->task = task_kernel_task();
	t->task->main_thread = t;

	thread_priority_init(t, SCHED_PRIORITY_MIN);

	cpu_init(cpu_get_id(), t);

	return t;
}
