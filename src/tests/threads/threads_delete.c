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
static struct thread *main;
static struct thread *minus;
static struct thread *mult;
static struct thread *plus_thread;

EMBOX_TEST(run);

/**
 * Endlessly writes "-".
 * Will be stopped and deleted.
 */
static void *minus_run(void *arg) {
	thread_yield();
	/* NOTREACHED */
	assert(false);
	while (true) {
		TRACE("-");
	}

	return NULL;
}

/**
 * Endlessly writes "*".
 * Will be stopped but not deleted.
 */
static void *mult_run(void *arg) {
	thread_yield();
	/* NOTREACHED */
	assert(false);
	while (true) {
		TRACE("*");
	}

	return NULL;
}

/**
 * Endlessly writes "+".
 * Will be started in natural_thread.
 */
static void *plus_run(void *arg) {
	size_t i;
	for (i = 1; i < 1000; i++) {
		TRACE("+");
	}

	return NULL;
}

/**
 * Stops minus_run and mult_run and starts plus_run.
 * Writes natural numbers.
 */
static void *main_run(void *arg) {
	size_t i;

	thread_stop(minus);
	thread_stop(mult);
	plus_thread = thread_alloc();
	thread_init(plus_thread, plus_run, plus_stack, THREAD_STACK_SIZE);
	assert(plus_thread);
	thread_start(plus_thread);

	for (i = 1; i < 300; i++) {
		TRACE("%d ", i);
	}

	return NULL;
}

static int run(void) {
	main = thread_alloc();
	mult = thread_alloc();
	minus = thread_alloc();

	thread_init(main, main_run, main_stack, THREAD_STACK_SIZE);
	thread_init(mult, mult_run, mult_stack, THREAD_STACK_SIZE);
	thread_init(minus, minus_run, minus_stack, THREAD_STACK_SIZE);

	assert(main);
	assert(minus);
	assert(mult);

	thread_start(main);
	thread_start(minus);
	thread_start(mult);

	thread_join(main);
	thread_join(mult);
	thread_join(minus);

	thread_free(main);
	thread_free(mult);
	thread_free(minus);

	return 0;
}
