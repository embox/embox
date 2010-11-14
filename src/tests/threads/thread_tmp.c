/**
 * @file
 * @brief studying threads
 *
 * @date 11/2010
 * @author Muhin Vladimir
 */

#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

static char inc_stack[THREAD_STACK_SIZE];

static struct thread *inc_thread;

EMBOX_TEST(run);

static void inc_run(void) {
	int i;
	for (i = 0; i < 256; i++) {
		TRACE("%d", i);
		TRACE(" ");
	}
}

static int run() {
	TRACE("\nHi! It's 256-thread!\n");
	inc_thread = thread_create(inc_run, inc_stack + THREAD_STACK_SIZE);
	assert(inc_thread != NULL);
	thread_start(inc_thread);
	scheduler_start();
	scheduler_stop();
	return 0;
}
