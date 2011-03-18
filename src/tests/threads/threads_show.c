/**
 * @file
 * @brief Tests function thread_yield.
 * @details Test, which a lot of times writes "+", "-", "*" and "!".
 * Because of priorities they are printed in such order:
 * 1) "+" and "+".
 * 2) "!";
 * 3) "*";
 * For each symbol there is a thread, which shows it.
 * These threads show one symbol and switch context.
 *
 * @date 09.05.10
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

static char plus_stack[THREAD_STACK_SIZE];
static char minus_stack[THREAD_STACK_SIZE];
static char mult_stack[THREAD_STACK_SIZE];
static char highest_stack[THREAD_STACK_SIZE];
static struct thread *plus_thread;
static struct thread *minus_thread;
static struct thread *mult_thread;
static struct thread *highest_thread;

EMBOX_TEST(run);

/**
 * Endlessly Writes "!". Thread with the highest priority.
 */
static void highest_run(void) {
	size_t i;
	for (i = 0; i < 100; i++) {
		TRACE("!");
		thread_yield();
	}
}

/**
 * Endlessly Writes "+".
 */
static void plus_run(void) {
	size_t i;
	for (i = 0; i < 100; i++) {
		TRACE("+");
		thread_yield();
	}
}

/**
 * Endlessly writes "-".
 */
static void minus_run(void) {
	size_t i;
	for (i = 0; i < 110; i++) {
		TRACE("-");
		thread_yield();
	}
}

/**
 * Endlessly writes "*".
 */
static void mult_run(void) {
	size_t i;
	for (i = 0; i < 120; i++) {
		TRACE("*");
		thread_yield();
	}
}

static int run(void) {
	TRACE("\n");

	plus_thread = thread_create(plus_run, plus_stack + THREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + THREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + THREAD_STACK_SIZE);
	highest_thread = thread_create(highest_run, highest_stack + THREAD_STACK_SIZE);
	highest_thread->priority = 2;
	plus_thread->priority = 3;
	minus_thread->priority = 3;
	assert(plus_thread != NULL);
	assert(minus_thread != NULL);
	assert(highest_thread != NULL);

	thread_start(plus_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);
	thread_start(highest_thread);

	TRACE("\nBefore start\n");
	scheduler_start();
	scheduler_stop();
	return 0;
}
