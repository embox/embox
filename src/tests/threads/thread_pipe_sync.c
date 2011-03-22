/**
 * @file
 * @brief
 *
 * @date 27.11.10
 * @author Muhin Vladimir
 */

#include <embox/test.h>
#include <embox/unit.h>
#include <kernel/thread/api.h>
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

static void first_run(void) {
	TRACE("\n First thread: \n");
	pipe_write(0, 'k');
	pipe_write(0, 'z');
	thread_yield();
}

static void second_run(void) {
	TRACE("\n Second thread: \n");
	pipe_write(0, 'm');
	char tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);

	pipe_write(0, 's');
	pipe_write(0, 't');
	pipe_write(0, 'a');
	pipe_write(0, 'r');
	pipe_write(0, 't');
	thread_yield();
}

static void third_run(void) {
	TRACE("\n Third thread: \n");
	char tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
	tmp = pipe_read(0);
	TRACE("\n tmp %c \n", tmp);
}

static int run(void) {
	third_thread = thread_create(third_run, third_stack
					+ THREAD_STACK_SIZE);
	second_thread = thread_create(second_run, second_stack
					+ THREAD_STACK_SIZE);
	first_thread = thread_create(first_run, first_stack
					+ THREAD_STACK_SIZE);

	assert(third_thread != NULL);
	assert(second_thread != NULL);
	assert(first_thread != NULL);
	thread_start(first_thread);
	thread_start(third_thread);
	thread_start(second_thread);

	pipe_create(); // pipe #0
	pipe_create(); // pipe #1

	scheduler_start();
	scheduler_stop();

	return 0;
}
