/**
 * @file
 * @brief Testing mutexes.
 *
 * @date 30.06.2010
 * @author Skorodumov Kirill
 */

#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <kernel/mutex.h>
#include <errno.h>
#include <assert.h>
#include <lib/list.h>

#define THREAD_STACK_SIZE 0x1000

static char plus_stack[THREAD_STACK_SIZE];
static char minus_stack[THREAD_STACK_SIZE];
static char mult_stack[THREAD_STACK_SIZE];
static char div_stack[THREAD_STACK_SIZE];


struct thread *plus_thread;
struct thread *minus_thread;
struct thread *mult_thread;
struct thread *div_thread;
struct thread *highest_thread;


struct mutex mutex;

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
 * Locks mutex end writes "-"
 */
static void minus_run(void) {
	int i;
	mutex_lock(&mutex);
	for (i = 0; i < 1000; i++) {
		TRACE("-");
	}
	mutex_unlock(&mutex);
	while (true) {
		TRACE("-");
	}
}


/**
 * Locks and writes '*'
 */
static void mult_run(void) {
	int i;
	mutex_lock(&mutex);
	for (i = 0; i < 1000; i++) {
			TRACE("*");
	}
	mutex_unlock(&mutex);
	while (true) {
		TRACE("*");
	}
}

/**
 * Endlessly writes "/"
 */
static void div_run(void) {
	int i;
	for (i = 0; i < 100; i++) {
		TRACE("/");
	}
}

/**
 * Test which writes "+","-","*" and "/"
 * minus_thread and mult_thread uses same mutex
 * to test how it works.
 * @return 0 if test finishes successfully.
 */
static int run_test(void) {
	TRACE("\n");

	mutex_init(&mutex);

	plus_thread = thread_create(plus_run, plus_stack + THREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + THREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + THREAD_STACK_SIZE);
	div_thread = thread_create(div_run, div_stack + THREAD_STACK_SIZE);

	assert(plus_thread != NULL);
	assert(minus_thread != NULL);
	assert(mult_thread != NULL);
	assert(div_thread != NULL);

	thread_start(plus_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);
	thread_start(div_thread);

	TRACE("\nBefore start\n");
	scheduler_start();
	scheduler_stop();
	return 0;
}
