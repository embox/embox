/**
 * @file
 * @brief Tests function heap_scheduler.
 * @details Run threads in order with decreasing priorities.
 *
 * @date 11.05.10
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x100
#define THREADS_COUNT 3

static char stacks[THREADS_COUNT][THREAD_STACK_SIZE];
static struct thread *threads[THREADS_COUNT];

EMBOX_TEST(run);

/**
 * Writes id of current thread.
 */
static void *threads_run(void *arg) {
	size_t i;
	for (i = 0; i < 10; i++) {
		TRACE("%d ", thread_self()->id);
	}

	return NULL;
}

static int run(void) {
	size_t i;

	for (i = 0; i < THREADS_COUNT; i++) {
		threads[i] = thread_alloc();
		thread_init(threads[i], threads_run, stacks[i], THREAD_STACK_SIZE);
		assert(threads[i]);
		threads[i]->priority = i + 1;
		thread_start(threads[i]);
	}

	for (i = 0; i < THREADS_COUNT; i++) {
		thread_join(threads[i]);
	}

	for (i = 0; i < THREADS_COUNT; i++) {
		thread_free(threads[i]);
	}

	return 0;
}
