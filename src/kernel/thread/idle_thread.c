/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */
#include <util/dlist.h>

#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_alloc.h>
#include <hal/arch.h> /*only for arch_idle */
#include <sys/types.h>
#include <kernel/cpu.h>

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
	struct thread *idle;

	if (!(idle = thread_alloc())) {
		return NULL;
	}


	thread_init(idle, THREAD_FLAG_NOTASK_THREAD, idle_run, NULL);
	idle->task = task_kernel_task();
	idle->task->main_thread = idle;

	thread_priority_set(idle, THREAD_PRIORITY_MIN);

	cpu_init(cpu_get_id(), idle);

	return idle;
}
