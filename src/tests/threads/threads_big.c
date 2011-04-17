/**
 * @file
 * @brief Tests threads
 * @details Test, which infinitely writes "?", "+", "*" and
 * natural numbers on the screen.
 * Thread, which writes "-", will be deleted in plus_thread.
 *
 * @date 18.04.10
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000
#define TOTAL_ITERATIONS 200

static char plus_stack[THREAD_STACK_SIZE];
static char minus_stack[THREAD_STACK_SIZE];
static char mult_stack[THREAD_STACK_SIZE];
static char natural_stack[THREAD_STACK_SIZE];
static struct thread *minus_t;

EMBOX_TEST(run);

/**
 * Writes "+".
 * Deletes minus_thread.
 */
static void *plus_run(void *arg) {
	thread_stop(minus_t);
	thread_yield();
	TRACE("+");

	return NULL;
}

/**
 * Endlessly writes "-".
 * Will be deleted.
 */
static void *minus_run(void *arg) {
	size_t i;
	assert(critical_allows(CRITICAL_PREEMPT));
	for (i = 0; i < TOTAL_ITERATIONS; i++) {
		TRACE("-");
	}

	return NULL;
}

/**
 * Endlessly writes "*".
 */
static void *mult_run(void *arg) {
	size_t i;
	for (i = 0; i < TOTAL_ITERATIONS; i++) {
		TRACE("  *");
	}

	return NULL;
}

/**
 * Endlessly writes natural numbers.
 */
static void *natural_run(void *arg) {
	size_t i;
	for (i = 1; i < TOTAL_ITERATIONS; i++) {
		TRACE("%d ", i);
	}

	return NULL;
}

static int run(void) {
	struct thread *plus_t;
	struct thread *mult_t;
	struct thread *natural_t;

	plus_t = thread_alloc();
	minus_t = thread_alloc();
	mult_t = thread_alloc();
	natural_t = thread_alloc();

	assert(plus_t != NULL);
	assert(minus_t != NULL);
	assert(mult_t != NULL);
	assert(natural_t != NULL);

	thread_init(plus_t, plus_run, plus_stack, THREAD_STACK_SIZE);
	thread_init(natural_t, natural_run, natural_stack, THREAD_STACK_SIZE);
	thread_init(minus_t, minus_run, minus_stack, THREAD_STACK_SIZE);
	thread_init(mult_t, mult_run, mult_stack, THREAD_STACK_SIZE);

	thread_start(plus_t);
	thread_start(natural_t);
	thread_start(minus_t);
	thread_start(mult_t);

	thread_join(plus_t, NULL);
	thread_join(minus_t, NULL);
	thread_join(mult_t, NULL);
	thread_join(natural_t, NULL);

	thread_free(plus_t);
	thread_free(minus_t);
	thread_free(mult_t);
	thread_free(natural_t);

	return 0;
}
