/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */
#include <time.h>

#include <kernel/thread.h>
#include <kernel/task.h>


extern void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg);


static void *boot_stub(void *arg) {
	return NULL;
}
/*
 * Initializes thread structure for current thread, adds it to list of threads
 * and to kernel task. Use this ONLY for bootstrap threads.
 */
struct thread *thread_init_self(void *stack, size_t stack_sz,
		thread_priority_t priority) {
	struct thread *thread = stack; /* Allocating at the bottom */

	/* Stack setting up */
	thread->stack = stack + sizeof(struct thread);
	thread->stack_sz = stack_sz - sizeof(struct thread);

	/* General initialization and task setting up */
	thread_init(thread, 0, boot_stub, NULL);

	/* Priority setting up */
	thread_priority_set(thread, priority);

	/* running time */
	thread->running_time = clock();
	thread->last_sync = thread->running_time;

	return thread;
}

struct thread *thread_boot_init(void) {
	struct thread *bootstrap;
	struct task *kernel_task = task_kernel_task();
	extern char _stack_vma, _stack_len;

	bootstrap = thread_init_self(&_stack_vma, (uint32_t) &_stack_len,
			THREAD_PRIORITY_NORMAL);

	task_add_thread(kernel_task, bootstrap);

	return bootstrap;
}
