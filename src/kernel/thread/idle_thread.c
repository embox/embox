/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */
#include <util/dlist.h>

#include <kernel/task.h>
#include <kernel/thread.h>
#include <hal/arch.h> /*only for arch_idle */

extern void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg);

extern struct thread *thread_alloc(void);
//extern void thread_free(struct thread *t);

//extern struct dlist_head __thread_list;

/*
 * Function, which does nothing. For idle_thread.
 */
static void *idle_run(void *arg) {
	while (true) {
		arch_idle();
	}
	return NULL;
}


struct thread *thread_idle_init(void) {
	struct thread *idle;

	if (!(idle = thread_alloc())) {
		return NULL;
	}


	thread_init(idle, THREAD_FLAG_TASK_THREAD, idle_run, NULL);
	idle->task = task_kernel_task();
	idle->task->main_thread = idle;


	thread_priority_set(idle, THREAD_PRIORITY_MIN);

	cpu_set_idle_thread(idle);

	return idle;
}
