/**
 * @file
 * @brief Implementation of methods in thread.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#include <kernel/thread.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <errno.h>
#include <kernel/scheduler.h>

#define MAX_THREADS_COUNT 32

/**
 * Pool, containing threads.
 */
static struct thread threads_pool[MAX_THREADS_COUNT];

void thread_run(int thread_pointer) {
	struct thread *running_thread = (struct thread *) thread_pointer;
	TRACE("\nThread ID = %d\n", running_thread->id);
	running_thread->run();
//	thread_delete(running_thread);
}

/**
 * A mask, which shows, what places for new threads are free.
 */
static int mask = 0;

struct thread * thread_new(void) {
	for (int i = 0; i < MAX_THREADS_COUNT; i++) {
		if (((mask >> i) & 1) == 0) {
			struct thread *created_thread;
			created_thread = threads_pool + i;
			created_thread->id = i + 1;
			TRACE("Thread ID = %d\n", created_thread->id);
			mask |= (1 << i);
			return created_thread;
		}
	}
	return NULL;
}

int thread_create(struct thread *created_thread, void run(void),
		void *stack_address) {
	if (created_thread == NULL || run == NULL || stack_address == NULL) {
		return -EINVAL;
	}
	created_thread->run = run;
	context_init(&created_thread->thread_context, true);
	context_set_entry(&created_thread->thread_context, thread_run,
			(int) created_thread);
	context_set_stack(&created_thread->thread_context, stack_address);
	return 0;
}

int thread_delete(struct thread *deleted_thread) {
	if (deleted_thread == NULL) {
		return -EINVAL;
	}
	scheduler_remove(deleted_thread);
	mask &= ~(1 << (deleted_thread - threads_pool));
	return 0;
}

