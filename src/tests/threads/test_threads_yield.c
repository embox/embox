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
static struct thread *plus_thread;
static struct thread *minus_thread;
static struct thread *mult_thread;

EMBOX_TEST(run_test)
;

/**
 * Endlessly Writes "+" and calls minus_run.
 */
static void plus_run(void) {
	int i;
	for (i = 0; i < 100; i++) {
		TRACE("+ ");
		thread_yield();
	}
}

/**
 * Endlessly writes "-" and calls plus_run.
 */
static void minus_run(void) {
	int i;
	for (i = 0; i < 100; i++) {
		TRACE("- ");
		thread_yield();
	}
}

/**
 * Endlessly writes "*" and calls plus_run.
 */
static void mult_run(void) {
	int i;
	for (i = 0; i < 100; i++) {
		TRACE("* ");
		thread_yield();
	}
}

/**
 * Test, which infinitely writes "+", "-".
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

	assert(plus_thread != NULL);
	assert(minus_thread != NULL);

	thread_start(plus_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);

	TRACE("\nBefore start\n");
	scheduler_start();

	return 0;
}
