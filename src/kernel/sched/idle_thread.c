/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.12.2014
 */

#include <util/err.h>
#include <hal/arch.h>

#include <kernel/cpu/cpu.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task.h>
#include <kernel/thread.h>

static void * idle_run(void *arg) {
	while (1) {
		arch_idle();
	}

	return NULL;
}

int idle_thread_create(void) {
	struct thread *t;

	t = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED, idle_run, NULL);
	if (err(t)) {
		return err(t);
	}

	task_thread_register(task_kernel_task(), t);
	schedee_priority_set(&t->schedee, SCHED_PRIORITY_MIN);

	cpu_init(cpu_get_id(), t);
	thread_launch(t);

	return 0;
}
