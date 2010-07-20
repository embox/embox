/**
 * @file
 * @brief Testing functions thread_lock and thread_unlock
 *
 * @date 30.06.2010
 * @author Skorodumov Kirill
 */

#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>
#include <lib/list.h>

#define THREAD_STACK_SIZE 0x1000

static char plus_stack[THREAD_STACK_SIZE];
static char minus_stack[THREAD_STACK_SIZE];
static char mult_stack[THREAD_STACK_SIZE];
static char div_stack[THREAD_STACK_SIZE];
static char highest_stack[THREAD_STACK_SIZE];


struct thread *plus_thread;
struct thread *minus_thread;
struct thread *mult_thread;
struct thread *div_thread;
struct thread *highest_thread;


struct event event;

EMBOX_TEST(run_test)
;


/**
 * endlessly writes '+'
 */
static void plus_run(void) {
	int i;
	for (i = 0; i < 1000; i++) {
		TRACE("+");
	}
}

/**
 * goes to sleep
 */
static void minus_run(void) {
	int i;
	scheduler_sleep(&event);
	for (i = 0; i < 1000; i++) {
		TRACE("-");
	}
}


/**
 * endlessly writes '-'
 */
static void mult_run(void) {
	int i;
	for (i = 0; i < 1000; i++) {
		TRACE("*");
	}
}

/**
 *
 */
static void div_run(void) {
	int i;
	thread_start(highest_thread);
	for (i = 0; i < 1000; i++) {
		TRACE("/");
	}
}


/**
 * Unlocks minus thread then writes "!" then yields 100 times. Thread with the highest priority.
 */
static void highest_run(void) {
	int i;
	scheduler_wakeup(&event);
	for (i = 0; i < 100; i++) {
		TRACE("!");
		thread_yield();
	}
}

/**
 * Test which writes "+","-","*" and "/"
 * Minus_thread goes to sleep then will be awaken
 * by highest_thread. Highest_thread is started by div_thread
 * and has highest priority.
 * @return 0 if test finishes successfully.
 */
static int run_test(void) {
	TRACE("\n");

	event_init(&event);

	plus_thread = thread_create(plus_run, plus_stack + THREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + THREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + THREAD_STACK_SIZE);
	div_thread = thread_create(div_run, div_stack + THREAD_STACK_SIZE);
	highest_thread = thread_create(highest_run, highest_stack + THREAD_STACK_SIZE);

	scheduler_remove(highest_thread);

	assert(plus_thread != NULL);
	assert(minus_thread != NULL);
	assert(mult_thread != NULL);
	assert(div_thread != NULL);
	assert(highest_thread != NULL);

	highest_thread->priority = 2;

	thread_start(plus_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);
	thread_start(div_thread);

	TRACE("\nBefore start\n");
	scheduler_start();
	scheduler_stop();
	return 0;
}
