/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */

#include <err.h>
#include <hal/arch.h> /*only for arch_idle */
#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>
#include <kernel/thread/priority_priv.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>

/*
 * Function, which does nothing. For idle_thread.
 */
static void * idle_run(void *arg) {
	while (1) {
		arch_idle();
	}

	return NULL;
}

struct thread * idle_thread_create(void) {
	struct thread *t;

	t = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
			idle_run, NULL);
	if (0 != err(t)) {
		return NULL;
	}
	task_thread_register(task_kernel_task(), t);
	thread_set_priority(t, SCHED_PRIORITY_MIN);
	cpu_init(cpu_get_id(), t);
	thread_launch(t);

	return t;
}
