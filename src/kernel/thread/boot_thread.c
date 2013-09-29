/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */
#include <time.h>
#include <stdint.h>

#include <kernel/thread.h>
#include <kernel/thread/state.h>
#include <kernel/task.h>

#include <module/embox/kernel/stack.h>

#define STACK_SZ \
 	OPTION_MODULE_GET(embox__kernel__stack, NUMBER, stack_size)

static void *boot_stub(void *arg) {
	return NULL;
}

/*
 * Initializes thread structure for current thread, adds it to list of threads
 * and to kernel task. Use this ONLY for bootstrap threads.
 */
struct thread *thread_init_self(void *stack, size_t stack_sz,
		sched_priority_t priority) {
	struct thread *thread = stack; /* Allocating at the bottom */

	/* Stack setting up */
	//thread->stack = stack + sizeof(struct thread);
	//thread->stack_sz = stack_sz - sizeof(struct thread);
	thread_stack_set(thread, stack + sizeof(struct thread));
	thread_stack_set_size(thread, stack_sz - sizeof(struct thread));



	/* General initialization and task setting up */
	thread_init(thread, 0, boot_stub, NULL);

	/* Priority setting up */
	thread_priority_init(thread, priority);

	/* setup state
	 * this thread must be active and not sleep
	 */
	thread->state = thread_state_do_activate(thread->state);
	thread->state = thread_state_do_oncpu(thread->state);
	thread_set_current(thread);

	return thread;
}

struct thread *boot_thread_create(void) {
	struct thread *bootstrap;
	struct task *kernel_task = task_kernel_task();
	extern char __stack;

	bootstrap = thread_init_self(&__stack, (size_t) STACK_SZ,
			THREAD_PRIORITY_NORMAL);

	task_add_thread(kernel_task, bootstrap);

	return bootstrap;
}
