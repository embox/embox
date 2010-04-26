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

#define TREAD_STACK_SIZE 0x1000


static char plus_stack[TREAD_STACK_SIZE];
static char minus_stack[TREAD_STACK_SIZE];
static struct thread *plus_thread;
static struct thread *minus_thread;

EMBOX_TEST(run_test);

/**
 * Endlessly writes "-".
 */
static void minus_run(void) {
	while (true) {
		TRACE("-");
	}
//	TRACE("-");
}

/**
 * Endlessly writes "+".
 */
static void plus_run(void) {
	while (true) {
		TRACE("+");
	}
}

/**
 * Test, which infinitely writes "+" and "-" on the screen.
 *
 * @retval 0 if test is passed
 * @retval -EINVAL if an error occurs.
 */
static int run_test() {
	scheduler_init();
	plus_thread = thread_new();
	minus_thread = thread_new();
	thread_create(plus_thread, plus_run, plus_stack + sizeof(plus_stack));
	thread_create(minus_thread, minus_run, minus_stack + sizeof(minus_stack));
	scheduler_add(plus_thread);
	scheduler_add(minus_thread);
	if (plus_thread == NULL) {
		TRACE("\nPlus thread is NULL\n");
		return -EINVAL;
	}
	if (minus_thread == NULL) {
		TRACE("\nMinus thread is NULL\n");
		return -EINVAL;
	}
	TRACE("\nPlus thread is not NULL\n");
	TRACE("Minus thread is not NULL\n");
	TRACE("\nBefore start\n");
	scheduler_start();
	TRACE("\nAfter start\n");
	return 0;
}
