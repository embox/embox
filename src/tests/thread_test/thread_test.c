/**
 * @file
 * @brief Tests threads.
 *
 * @date 18.04.2010
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>

#define TREAD_STACK_SIZE 0x1000

static char plus_stack[TREAD_STACK_SIZE];
static char minus_stack[TREAD_STACK_SIZE];
static char mult_stack[TREAD_STACK_SIZE];
static struct thread *plus_thread;
static struct thread *minus_thread;
static struct thread *mult_thread;

EMBOX_TEST(run_test)
;

/**
 * Writes "+".
 * Deletes minus_thread.
 */
static void plus_run(void) {
	thread_stop(minus_thread);
	/*	while (true) {
	 TRACE("+");
	 }
	 */TRACE("+");
}

/**
 * Endlessly writes "-".
 * Will be deleted.
 */
static void minus_run(void) {
	while (true) {
		TRACE("-");
	}
}

/**
 * Endlessly writes "*".
 */
static void mult_run(void) {
	while (true) {
		TRACE("*");
	}
}

/**
 * Test, which infinitely writes "?", "+" and "*" on the screen.
 * Thread, which writes "-", will be deleted in plus_thread.
 *
 * It still doesn't work.
 *
 * @retval 0 if test is passed
 * @retval -EINVAL if an error occurs.
 */
static int run_test() {
	TRACE("\n");
	scheduler_init();

	plus_thread = thread_create(plus_run, plus_stack + TREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + TREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + TREAD_STACK_SIZE);

	assert(plus_thread != NULL);
	assert(minus_thread != NULL);
	assert(mult_thread != NULL);

	thread_start(plus_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);

	TRACE("\nBefore start\n");
	scheduler_start();
	TRACE("\nAfter start\n");

	return 0;
}
