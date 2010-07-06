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

struct thread *plus_thread;
struct thread *minus_thread;
struct thread *mult_thread;

struct event event;

EMBOX_TEST(run_test)
;


/**
 * endlessly writes '+'
 */
static void plus_run(void) {
	while (true) {
		TRACE("+");
	}
}

/**
 * endlessly writes '-'
 * then goes to sleep
 */
static void minus_run(void) {
	scheduler_sleep(minus_thread, &event);
	while (true) {
		TRACE("-");
	}
}


/**
 * endlessly writes '-'
 * unlocks minus_thread
 */
static void mult_run(void) {
//	scheduler_wakeup(&event);
	while (true) {
		TRACE("*");
	}
}


static int run_test(void) {

	TRACE("\n");

	scheduler_init();

	INIT_LIST_HEAD(&event.threads_list);

	plus_thread = thread_create(plus_run, plus_stack + THREAD_STACK_SIZE);
	minus_thread = thread_create(minus_run, minus_stack + THREAD_STACK_SIZE);
	mult_thread = thread_create(mult_run, mult_stack + THREAD_STACK_SIZE);

	assert(plus_thread != NULL);
	assert(minus_thread != NULL);
	assert(mult_thread != NULL);

	thread_start(plus_thread);
	thread_start(minus_thread);
	thread_start(mult_thread);

	TRACE("\nBefore start\n");
	scheduler_start();

	return 0;
}
