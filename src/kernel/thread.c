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

/**
 * If it doesn't equal to zero, it means
 * that we are located in critical section
 * and cant's switch between threads.
 */
static int preemption_count;

void thread_run(int thread_pointer) {
	struct thread *running_thread = (struct thread *)thread_pointer;
	running_thread->run();
	TRACE("\nWhat's the ****?\n");
//	thread_delete(running_thread);
}


#define THREAD_STACK_SIZE 0x1000
static char idle_thread_stack[THREAD_STACK_SIZE];
void threads_init(void) {
	preemption_count = 0;
	thread_create(&idle_thread, idle_run,
			idle_thread_stack + sizeof(idle_thread_stack));
	TRACE("adasdasd\n");
}

int thread_create(struct thread *created_thread, void run(void),
		void *stack_address) {
	if (created_thread == NULL || run == NULL || stack_address == NULL) {
		return EINVAL;
	}
	created_thread->run = run;
	context_init(&created_thread->thread_context, true);
	context_set_entry(&created_thread->thread_context, thread_run, (int)created_thread);
	context_set_stack(&created_thread->thread_context, stack_address);
	return 0;
}

void thread_delete (struct thread *deleted_thread) {
#if 0
	/**TODO*/
	if (deleted_thread == NULL) {
		return 1;
	}
	/*
	return 0;
	*/
#endif
}

void idle_run(void) {
	while (true) {
		TRACE("+");
	}
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
