/**
 * @file
 * @brief
 *
 * @date 21.11.2010
 * @author Muhin Vladimir
 */

#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/pipe.h>
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

static struct thread *thread_array[3] = {first_thread, second_thread, third_thread};

const int n = 50;

static void first_run(void){
	TRACE("\n First thread: \n");
	for (int i = 0; i < 10; i++){
		TRACE("%d ", i);
	}
	synchronize(3, i, first_thread);
}

static void second_run(void){
	TRACE("\n Second thread: \n");
	for (int i = 0; i < 10; i++){
		TRACE("%d ", i);
	}
	synchronize(3, i, second_thread);
}

static void third_run(void){
	TRACE("\n third thread: \n");
	for (int i = 0; i < 10; i++){
		TRACE("%d ", i);
	}
	synchronize(3, i, third_thread);

}

static int run(){
	first_thread = thread_create(first_run, first_stack + THREAD_STACK_SIZE);
	second_thread = thread_create(second_run, second_stack + THREAD_STACK_SIZE);
	third_thread = thread_create(third_run, third_stack + THREAD_STACK_SIZE);

	assert(first_thread != NULL);
	assert(second_thread != NULL);
	assert(third_thread != NULL);

	thread_start(first_thread);
	thread_start(second_thread);
	thread_start(third_thread);

	new_pipe = pipe_create(3);

	scheduler_start();
	scheduler_stop();

	return 0;
}
