/**
 * @file
 * @brief Tests function thread_yield.
 *
 * @date 09.05.2010
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/heap_scheduler.h>
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

EMBOX_TEST(run_test)
;

/**
 * Endlessly Writes "!". Thread with the highest priority.
 */
static void highest_run(void) {
	int i;
	for (i = 0; i < 100; i++) {
		TRACE("!");
		thread_yield();
	}
}

/**
 * Endlessly Writes "+".
 */
static void plus_run(void) {
	int i;
	for (i = 0; i < 100; i++) {
		TRACE("+");
		thread_yield();
	}
}

/**
 * Endlessly writes "-".
 */
static void minus_run(void) {
	int i;
	for (i = 0; i < 110; i++) {
		TRACE("-");
		thread_yield();
	}
}

/**
 * Endlessly writes "*".
 */
static void mult_run(void) {
	int i;
	for (i = 0; i < 120; i++) {
		TRACE("*");
		thread_yield();
	}
}

/**
 * Test, which infinitely writes "+", "-" and "*".
 * Before them he ran thread with "!" because it has the highest priority.
 * This threads show one symbol and switch context.
 *
 * @retval 0 if test is passed
 * @retval -EINVAL if an error occurs.
 */
static int run_test() {
	TRACE("\n");
	scheduler_init();

	plus_thread = thread_create(plus_run, plus_stack + THREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + THREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + THREAD_STACK_SIZE);
	highest_thread = thread_create(highest_run, highest_stack + THREAD_STACK_SIZE);
	highest_thread->priority = 2;
	assert(plus_thread != NULL);
	assert(minus_thread != NULL);
	assert(highest_thread != NULL);

	thread_start(plus_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);
	thread_start(highest_thread);

	TRACE("\nBefore start\n");
	scheduler_start();

	return 0;
}
