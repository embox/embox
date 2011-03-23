/**
 * @file
 * @brief Testing functions sched_thread_lock and thread_unlock
 * @details Test which writes "+","-","*" and "/"
 * Minus_thread goes to sleep then will be awaken
 * by highest_thread. Highest_thread is started by div_thread
 * and has highest priority.
 *
 * @date 30.06.10
 * @author Skorodumov Kirill
 */

#include <embox/test.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
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

EMBOX_TEST(run);

/**
 * endlessly writes '+'
 */
static void plus_run(void) {
	size_t i;
	for (i = 0; i < 1000; i++) {
		TRACE("+");
	}
}

/**
 * goes to sleep
 */
static void minus_run(void) {
	size_t i;
	scher_sleep(&event);
	for (i = 0; i < 1000; i++) {
		TRACE("-");
	}
}

/**
 * endlessly writes '-'
 */
static void mult_run(void) {
	size_t i;
	for (i = 0; i < 1000; i++) {
		TRACE("*");
	}
}

static void div_run(void) {
	size_t i;
	thread_start(highest_thread);
	for (i = 0; i < 1000; i++) {
		TRACE("/");
	}
}

/**
 * Unlocks minus thread then writes "!" then yields 100 times. Thread with the highest priority.
 */
static void highest_run(void) {
	size_t i;
	TRACE("Highest");
	sched_wakeup(&event);
	TRACE("Highest cont");
	for (i = 0; i < 100; i++) {
		TRACE("!");
		thread_yield();
	}
}

static int run(void) {
	TRACE("\n");

	event_init(&event);

	plus_thread = thread_create(plus_run, plus_stack + THREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + THREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + THREAD_STACK_SIZE);
	div_thread = thread_create(div_run, div_stack + THREAD_STACK_SIZE);
	highest_thread = thread_create(highest_run, highest_stack + THREAD_STACK_SIZE);

	sched_remove(highest_thread);

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
	sched_start();
	sched_stop();
	return 0;
}
