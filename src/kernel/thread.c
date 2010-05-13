/**
 * @file
 * @brief Implementation of methods in thread.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#include <assert.h>
#include <errno.h>

#include <kernel/thread.h>
#include <kernel/heap_scheduler.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <hal/ipl.h>
#include <embox/unit.h>

#define MAX_THREADS_COUNT 32
#define THREAD_STACK_SIZE 0x10000

EMBOX_UNIT_INIT(threads_init);

struct thread *idle_thread;
struct thread *current_thread;

/** Stack for idle_thread. */
static char idle_thread_stack[THREAD_STACK_SIZE];

/**
 * Function, which makes nothing. For idle_thread.
 */
static void idle_run(void) {
	while (true) {
		idle_thread->reschedule = true;
	}
}

/** Last zombie thread. */
static struct thread *last_zombie;
/** Pool, containing threads. */
static struct thread threads_pool[MAX_THREADS_COUNT];

static int threads_init(void) {
	idle_thread = thread_create(idle_run, idle_thread_stack + THREAD_STACK_SIZE);
	idle_thread->priority = 0;
	idle_thread->state = THREAD_STATE_RUN;
	return 0;
}

/**
 * Transforms function "run" in thread into function which can
 * execute "run" and delete thread from scheduler.
 * @param thread_pointer pointer at thread.
 */
static void thread_run(int data) {
	struct thread *thread = (struct thread *) data;

	assert(thread != NULL);
	TRACE("\nStarting Thread %d\n", thread->id);

	ipl_enable();
	thread->run();
	thread_stop(thread);

	/* NOTREACHED */
	assert(false);
}

/** A mask, which shows, what places for new threads are free. */
static int mask = 0;

/**
 * Allocates memory for new thread.
 *
 * @return pointer to alloted thread
 * @retval NULL if there are not free threads
 */
static struct thread * thread_new(void) {
	struct thread *created_thread;
	for (int i = 0; i < MAX_THREADS_COUNT; i++) {
		if (((mask >> i) & 1) == 0) {
			created_thread = threads_pool + i;
			created_thread->id = i;
			//TRACE("Alloted thread ID = %d\n", created_thread->id);
			mask |= (1 << i);
			return created_thread;
		}
	}
	return NULL;
}

struct thread *thread_create(void (*run)(void), void *stack_address) {
	struct thread *created_thread = thread_new();
	if (created_thread == NULL || run == NULL || stack_address == NULL) {
		return NULL;
	}
	created_thread->state = THREAD_STATE_STOP;
	created_thread->run = run;
	created_thread->priority = 1;
	context_init(&created_thread->context, true);
	context_set_entry(&created_thread->context, &thread_run, (int) created_thread);
	context_set_stack(&created_thread->context, stack_address);
	return created_thread;
}

void thread_start(struct thread *thread) {
	thread->state = THREAD_STATE_RUN;
	scheduler_add(thread);
}

/**
 * Deletes chosen thread.
 */
static int thread_delete(struct thread *deleted_thread) {
	if (deleted_thread == NULL) {
		return -EINVAL;
	}
	TRACE("\nDeleting %d\n", deleted_thread->id);
	scheduler_remove(deleted_thread);
	deleted_thread->state = THREAD_STATE_STOP;
	mask &= ~(1 << (deleted_thread - threads_pool));
	return 0;
}

int thread_stop(struct thread *stopped_thread) {
	if (stopped_thread == NULL || stopped_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	TRACE("\nStopping %d\n", stopped_thread->id);
	if (last_zombie != NULL) {
		thread_delete(last_zombie);
	}
	if (current_thread != stopped_thread) {
		thread_delete(stopped_thread);
	} else {
		last_zombie = stopped_thread;
		stopped_thread->state = THREAD_STATE_ZOMBIE;
		stopped_thread->reschedule = true;
	}
	scheduler_unlock();
	return 0;
}

void thread_yield(void) {
	scheduler_lock();
	current_thread->reschedule = true;
	scheduler_unlock();
}
