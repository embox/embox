/**
 * @file
 * @brief Tests thread with recursion.
 * @details Test, which writes some numbers using recursion.
 * If MAX_DEPTH = 100, then error occurs (maybe stack overflow).
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
#define MAX_DEPTH 10
static char recursion_stack[THREAD_STACK_SIZE];
static struct thread *recursion_thread;

EMBOX_TEST(run);

/**
 * Shows natural number on the screen and maybe calls itself.
 * @param i shown number. if i < MAX_DEPTH f calls itself.
 */
static void f(int i) {
	TRACE("%d ", i);
	if (i < MAX_DEPTH) {
		f(i+1);
	}
	TRACE("-%d ", i);
}

/**
 * Starts recursion.
 * Shows natural numbers.
 */
static void recursion_run(void) {
	f(1);
}

static int run(void) {
	TRACE("\n");

	recursion_thread =
		thread_init(recursion_run, recursion_stack + THREAD_STACK_SIZE);

	assert(recursion_thread != NULL);
	thread_start(recursion_thread);

	TRACE("\nBefore start\n");
	sched_start();
	sched_stop();
	return 0;
}
