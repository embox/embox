/**
 * @file thread.c
 * @brief TODO
 *
 * @date 16.04.2010
 * @author Avdyukhin Dmitry
 */

#include <hal/thread.h>
#include <hal/context.h>

void * kmem_alloc(void * address, size_t size) {
	/**TODO*/
}

void kmem_free(void * address) {
	/**TODO*/
}

struct thread * thread_create(void (*run)(void), void * stack_address, size_t stack_size) {
	struct thread * created_thread = kmem_alloc(stack_address, sizeof(struct thread));
	context_init(&created_thread->thread_context, false);
	context_set_entry(&created_thread->thread_context, run);
	context_set_stack(&created_thread->thread_context, run);
	return created_thread;
}

void thread_delete (struct thread * deleted_thread) {
	kmem_free(&deleted_thread->thread_context);
	kmem_free(&deleted_thread);
}

void idle_run(void) {
	while (true)
		arch_idle();
}

void scheduler_lock(void) {
	preemption_count++;
}

void scheduler_unlock(void) {
	if (--preemption_count == 0)
		scheduler_dispatch();
}

void scheduler_dispatch(void) {
	/**TODO*/
}
