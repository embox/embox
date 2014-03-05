/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */

#include <embox/unit.h>
#include <err.h>
#include <hal/arch.h> /*only for arch_idle */
#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>
#include <kernel/thread.h>

#include <kernel/task/kernel_task.h>
#include <kernel/thread/thread_register.h>

EMBOX_UNIT_INIT(idle_thread_init);

/*
 * Function, which does nothing. For idle_thread.
 */
static void * idle_run(void *arg) {
	while (1) {
		arch_idle();
	}

	return NULL;
}


static int idle_thread_init(void) {
	struct thread *t;

	t = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
			idle_run, NULL);
	if (0 != err(t)) {
		return err(t);
	}

	thread_register(task_kernel_task(), t);

	thread_priority_init(t, SCHED_PRIORITY_MIN);

	cpu_init(cpu_get_id(), t);

	return 0;
}
