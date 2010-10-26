/**
 * @file
 * @brief Tests function heap_scheduler.
 * @details Run threads in order with decreasing priorities.
 *
 * @date 11.05.2010
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000
#define THREADS_COUNT 10

static char stacks[THREADS_COUNT][THREAD_STACK_SIZE];
static struct thread *threads[THREADS_COUNT];

EMBOX_TEST(run_test);

/**
 * Writes id of current thread.
 */
static void threads_run(void) {
	size_t i;
	for (i = 0; i < 10; i++) {
		TRACE("%d ", current_thread->id);
	}
}

static int run_test() {
	size_t i;
	TRACE("\n");

	for (i = 0; i < THREADS_COUNT; i++) {
		threads[i] = thread_create(threads_run, stacks[i] + THREAD_STACK_SIZE);
		assert(threads[i] != NULL);
		threads[i]->priority = i+1;
		thread_start(threads[i]);
	}

	TRACE("\nBefore start\n");
	scheduler_start();
	scheduler_stop();
	return 0;
}
