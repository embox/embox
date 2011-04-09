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

struct thread *plus;
struct thread *minus;
struct thread *mult;
struct thread *div;
struct thread *highest;

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
	sched_sleep(&event);
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
	thread_start(highest);
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
	sched_wake(&event);
	TRACE("Highest cont");
	for (i = 0; i < 100; i++) {
		TRACE("!");
		thread_yield();
	}
}

static int run(void) {
	TRACE("\n");

	event_init(&event, "test");

	plus = thread_alloc();
	minus = thread_alloc();
	mult = thread_alloc();
	div = thread_alloc();
	highest = thread_alloc();

	thread_init(plus, plus_run, plus_stack, THREAD_STACK_SIZE);
	thread_init(minus, minus_run, minus_stack, THREAD_STACK_SIZE);
	thread_init(mult, mult_run, mult_stack, THREAD_STACK_SIZE);
	thread_init(div, div_run, div_stack, THREAD_STACK_SIZE);
	thread_init(highest, highest_run, highest_stack, THREAD_STACK_SIZE);

	sched_remove(highest);

	assert(plus);
	assert(minus);
	assert(mult);
	assert(div);
	assert(highest);

	plus->priority = 2;
	minus->priority = 2;
	mult->priority = 2;
	div->priority = 2;
	highest->priority = 1;

	thread_start(plus);
	thread_start(minus);
	thread_start(mult);
	thread_start(div);

	thread_join(plus);
	thread_join(minus);
	thread_join(mult);
	thread_join(div);
	thread_join(highest);

	thread_free(plus);
	thread_free(minus);
	thread_free(mult);
	thread_free(div);
	thread_free(highest);

	return 0;
}
