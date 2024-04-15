/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.12.2014
 */
#include <stddef.h>

#include <hal/platform.h>
#include <kernel/cpu/cpu.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>
#include <util/err.h>
#include <util/log.h>

static void *idle_run(void *arg) {
	while (1) {
		platform_idle();
	}

	return NULL;
}

int idle_thread_create(void) {
	struct thread *t;

	t = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED, idle_run,
	    NULL);
	if (ptr2err(t)) {
		log_error(" Couldn't create thread err=%d", ptr2err(t));
		return ptr2err(t);
	}

	task_thread_register(task_kernel_task(), t);
	schedee_priority_set(&t->schedee, SCHED_PRIORITY_MIN);
	log_debug("idle_schedee = %#x", &t->schedee);

	cpu_init(cpu_get_id(), t);
	thread_launch(t);

	return 0;
}
