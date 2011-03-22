/**
 * @file
 * @brief We have 2 threads. When inc_thread start it makes increment
 *        and checking pipe at every iteration. When i becomes equal
 *        pipe data dec_thread starting.
 *
 * @date 14.11.10
 * @author Muhin Vladimir
 */

#include <embox/test.h>
#include <kernel/thread/api.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

static char inc_stack[THREAD_STACK_SIZE];
static char dec_stack[THREAD_STACK_SIZE];

static struct thread *inc_thread;
static struct thread *dec_thread;

const int flag = 50;

int pipe[2] = {50, 0};  //our small pipe :-)
			//first element is number of iterations before synchronize,
			//second - some addition data
EMBOX_TEST(run);

static void synchronize(void) {
	thread_stop(inc_thread);
}

static void inc_run(void) {
	int i;
	for (i = 0; i < 256; i++) {
		if (i == pipe[0]) {
			pipe[1] = i;
			synchronize();
		};
		TRACE("%d", i);
		TRACE(" ");
	}
}

static void dec_run(void) {
	int j, k = pipe[1];
	for (j = k; j > 0; j--) {
		TRACE("%d", j);
		TRACE(" ");
	}
}

static int run(void) {
	TRACE("\nHi! It's 256-thread!\n");
	inc_thread = thread_create(inc_run, inc_stack + THREAD_STACK_SIZE);
	dec_thread = thread_create(dec_run, dec_stack + THREAD_STACK_SIZE);
	assert(inc_thread != NULL);
	assert(dec_thread != NULL);
	thread_start(dec_thread);
	thread_start(inc_thread);
	scheduler_start();
	scheduler_stop();
	return 0;
}
