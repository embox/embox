/**
 * @file
 * @brief Tests functions thread_stop and thread_delete.
 * @details Test, which writes natural numbers.
 * Minus_thread and mult_run will be deleted.
 * Plus_run will be started in natural_run.
 * After deleting some threads:
 *   1)Plus_run id must be equal to 2.
 *   2)Must work only threads with id: 0, 1, 2.
 *
 * @date 09.05.10
 * @author Dmitry Avdyukhin
 */

#include <embox/test.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

static char main_stack[THREAD_STACK_SIZE];
static char minus_stack[THREAD_STACK_SIZE];
static char mult_stack[THREAD_STACK_SIZE];
static char plus_stack[THREAD_STACK_SIZE];
static struct thread *main_thread;
static struct thread *minus_thread;
static struct thread *mult_thread;
static struct thread *plus_thread;

EMBOX_TEST(run);

/**
 * Endlessly writes "-".
 * Will be stopped and deleted.
 */
static void minus_run(void) {
	thread_yield();
	/* NOTREACHED */
	assert(false);
	while (true) {
		TRACE("-");
	}
}

/**
 * Endlessly writes "*".
 * Will be stopped but not deleted.
 */
static void mult_run(void) {
	thread_yield();
	/* NOTREACHED */
	assert(false);
	while (true) {
		TRACE("*");
	}
}

/**
 * Endlessly writes "+".
 * Will be started in natural_thread.
 */
static void plus_run(void) {
	size_t i;
	for (i = 1; i < 1000; i++) {
		TRACE("+");
	}
}

/**
 * Stops minus_run and mult_run and starts plus_run.
 * Writes natural numbers.
 */
static void main_run(void) {
	size_t i;

	thread_stop(minus_thread);
	thread_stop(mult_thread);
	plus_thread = thread_create(plus_run, plus_stack + THREAD_STACK_SIZE);
	assert(plus_thread != NULL);
	thread_start(plus_thread);

	for (i = 1; i < 300; i++) {
		TRACE("%d ", i);
	}
}

static int run(void) {
	TRACE("\n");

	main_thread = thread_create(main_run, main_stack + THREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + THREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + THREAD_STACK_SIZE);

	assert(main_thread != NULL);
	assert(minus_thread != NULL);
	assert(mult_thread != NULL);

	thread_start(main_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);

	TRACE("\nBefore start\n");
	sched_start();
	sched_stop();
	return 0;
}
