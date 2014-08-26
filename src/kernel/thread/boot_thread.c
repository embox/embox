/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */

#include <assert.h>
#include <stdint.h>
#include <time.h>

#include <kernel/task/kernel_task.h>
#include <kernel/schedee/current.h>
#include <kernel/thread.h>
#include <kernel/task.h>

#include <module/embox/kernel/stack.h>

#define STACK_SZ \
	((size_t) OPTION_MODULE_GET(embox__kernel__stack, NUMBER, stack_size))

static void *boot_stub(void *arg) {
	assert(0, "Entering boot_stub");
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
	thread_stack_init(thread, stack_sz);

	/* General initialization and task setting up */
	thread_init(thread, 0, boot_stub, NULL);

	/* Priority setting up */
	thread_priority_init(thread, priority);

	/* setup state
	 * this thread must be active and not sleep
	 */
	thread->state = TS_INIT;

	thread->schedee.ready = true;
	thread->schedee.active = true;
	thread->schedee.waiting = false;

	schedee_set_current(&thread->schedee);

	return thread;
}

struct thread *boot_thread_create(void) {
	struct thread *bootstrap;
	extern char _stack_top;

	bootstrap = thread_init_self(&_stack_top - STACK_SZ, STACK_SZ,
			THREAD_PRIORITY_NORMAL);

	task_set_main(task_kernel_task(), bootstrap);

	return bootstrap;
}
