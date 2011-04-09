/**
 * @file
 * @brief Testing mutexes.
 * @details Test which writes "+","-","*" and "/"
 * minus_thread and mult_thread uses same mutex
 * to test how it works.
 *
 * @date 30.06.10
 * @author Skorodumov Kirill
 */

#include <embox/test.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sync/mutex.h>
#include <errno.h>
#include <assert.h>
#include <lib/list.h>

#define THREAD_STACK_SIZE 0x1000

static char plus_stack[THREAD_STACK_SIZE];
static char minus_stack[THREAD_STACK_SIZE];
static char mult_stack[THREAD_STACK_SIZE];
static char div_stack[THREAD_STACK_SIZE];

static struct thread *plus;
static struct thread *minus;
static struct thread *mult;
static struct thread *div;

static struct mutex mutex;

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
 * Locks mutex end writes "-"
 */
static void minus_run(void) {
	size_t i;
	mutex_lock(&mutex);
	for (i = 0; i < 500; i++) {
		TRACE("-");
	}
	TRACE("\nUnlock mutex by minus\n");
	mutex_unlock(&mutex);
	/* TODO If make i < 500, there will be an error.
	 * We think, it is a tsim's error. */
	for (i = 0; i < 1000; i++) {
		TRACE("-");
	}
}

/**
 * Locks and writes '*'
 */
static void mult_run(void) {
	size_t i;
	mutex_lock(&mutex);
	for (i = 0; i < 500; i++) {
			TRACE("*");
	}
	TRACE("\nUnlock mutex by mult\n");
	mutex_unlock(&mutex);
	for (i = 0; i < 500; i++) {
		TRACE("*");
	}
}

/**
 * Endlessly writes "/"
 */
static void div_run(void) {
	size_t i;
	for (i = 0; i < 100; i++) {
		TRACE("/");
	}
}

static int run(void) {
	mutex_init(&mutex);

	plus = thread_alloc();
	minus = thread_alloc();
	mult = thread_alloc();
	div = thread_alloc();

	thread_init(plus, plus_run, plus_stack + THREAD_STACK_SIZE);
	thread_init(minus, minus_run, minus_stack + THREAD_STACK_SIZE);
	thread_init(mult, mult_run, mult_stack + THREAD_STACK_SIZE);
	thread_init(div, div_run, div_stack + THREAD_STACK_SIZE);

	assert(plus);
	assert(minus);
	assert(mult);
	assert(div);

	thread_start(plus);
	thread_start(minus);
	thread_start(mult);
	thread_start(div);

	sched_start();

	thread_join(plus);
	thread_join(minus);
	thread_join(mult);
	thread_join(div);

	sched_stop();

	thread_free(plus);
	thread_free(minus);
	thread_free(mult);
	thread_free(div);

	return 0;
}
