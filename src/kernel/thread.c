/**
 * @file
 * @brief TODO
 *
 * @date 18.04.2010
 * @author Avdyukhin Dmitry
 */

#include <kernel/thread.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <errno.h>

struct thread idle_thread;
int preemption_count;

int thread_create(struct thread *created_thread, void (*run)(void),
		void *stack_address) {
	if (created_thread == NULL || run == NULL || stack_address == NULL) {
		return EINVAL;
	}
	context_init(&created_thread->thread_context, true);
	context_set_entry(&created_thread->thread_context, run);
	context_set_stack(&created_thread->thread_context, stack_address);
	return 0;
}

int thread_delete (struct thread *deleted_thread) {
	/**TODO*/
	if (deleted_thread == NULL) {
		return EINVAL;
	}
#if 0
	kmem_free(&deleted_thread);
#endif
	return 0;
}

static void idle_run(void) {
	while (true) {
		arch_idle();
	}
}

#define THREAD_STACK_SIZE 0x400
static int idle_thread_stack[THREAD_STACK_SIZE];

void threads_init(void) {
	preemption_count = 0;
	thread_create(&idle_thread, idle_run,
			idle_thread_stack + sizeof(idle_thread_stack));
}

void scheduler_lock(void) {
	preemption_count++;
}

void scheduler_unlock(void) {
	if (--preemption_count == 0) {
		scheduler_dispatch();
	}
}

void scheduler_dispatch(void) {
	/**TODO*/
}
