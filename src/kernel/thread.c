/**
 * @file thread.c
 * @brief TODO
 *
 * @date 16.04.2010
 * @author Avdyukhin Dmitry
 */

#include <kernel/thread.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <errno.h>

/**
 * Allots memory segment of chosen size
 * Returns address of alloted segment.
 */
void *kmem_alloc(size_t size) {
	/**TODO*/
	return NULL;
}

/**
 * Cleans memory with chosen address
 */
void kmem_free(void *address) {
	/**TODO*/
}

struct thread *idle_thread;
int preemption_count;

struct thread *thread_create(void (*run)(void),
		void *stack_address, size_t stack_size) {
	if (run == NULL || stack_size < 0 || (int)stack_address < (size_t)stack_size) {
		return NULL;
	}
	struct thread *created_thread =
		(struct thread *)kmem_alloc((size_t)sizeof(struct thread));
	context_init(&created_thread->thread_context, true);
	context_set_entry(&created_thread->thread_context, (void *)run);
	context_set_stack(&created_thread->thread_context, stack_address);
	return created_thread;
}

int thread_delete (struct thread *deleted_thread) {
	if (deleted_thread == NULL) {
		return EINVAL;
	}
	kmem_free(&deleted_thread);
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
	idle_thread = thread_create(idle_run, idle_thread_stack,
			(size_t)sizeof(idle_thread_stack));
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
