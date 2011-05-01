/**
 * @file
 * @brief
 *
 * @date 27.11.2010
 * @author Muhin Vladimir
 */

#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/pipe_manager.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

EMBOX_TEST(run);

static char first_stack[THREAD_STACK_SIZE];
static char second_stack[THREAD_STACK_SIZE];
static char third_stack[THREAD_STACK_SIZE];

static struct thread *first_thread;
static struct thread *second_thread;
static struct thread *third_thread;

static struct n_pipe *new_pipe;

static void first_run(void) {
	TRACE("\n First thread: \n");
	int i;
	i = pipe_section_read(0, 0);
	TRACE("\n i in pipe = %d", i);
	i++;
	TRACE("\n i++ = %d", i);
	pipe_section_write(0, 0, i);
	TRACE("\n i in pipe = %d\n", pipe_section_read(0, 0));
	thread_yield();
}

static void second_run(void) {
	TRACE("\n Second thread: \n");
	int j;
	j = pipe_section_read(0, 0);
	TRACE("\n j in pipe = %d", j);
	j++;
	TRACE("\n j++ = %d", j);
	pipe_section_write(0, 0, j);
	TRACE("\n j in pipe = %d\n", pipe_section_read(0,0));
	thread_yield();
}

static void third_run(void) {
	TRACE("\n Third thread: \n");
	int k;
	k = pipe_section_read(0, 0);
	TRACE("\n k in pipe = %d", k);
	k++;
	TRACE("\n k++ = %d", k);
	pipe_section_write(0, 0, k);
	TRACE("\n k in pipe = %d\n", pipe_section_read(0,0));
	thread_yield();

}

static int run() {
	third_thread =
			thread_create(third_run, third_stack
					+ THREAD_STACK_SIZE);
	second_thread =
			thread_create(second_run, second_stack
					+ THREAD_STACK_SIZE);
	first_thread =
			thread_create(first_run, first_stack
					+ THREAD_STACK_SIZE);

	assert(third_thread != NULL);
	assert(second_thread != NULL);
	assert(first_thread != NULL);

	thread_start(third_thread);
	thread_start(second_thread);
	thread_start(first_thread);


	pool_init();
	new_pipe = pipe_create(); // pipe #1
	scheduler_start();
	scheduler_stop();

	return 0;
}
