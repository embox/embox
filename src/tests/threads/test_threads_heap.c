/**
 * @file
 * @brief Tests function heap_scheduler.
 *
 * @date 11.05.2010
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/heap_scheduler.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000
#define threads_count 10

static char stacks[threads_count][THREAD_STACK_SIZE];
static struct thread *threads[threads_count];

EMBOX_TEST(run_test)
;
/**
 * Writes id of current thread.
 */
static void threads_run(void) {
	int i;
	for (i = 0; i < 10; i++) {
		TRACE("%d ", current_thread->id);
	}
}


/**
 * Run threads in order with decreasing priorities.
 *
 * @retval 0 if test is passed
 * @retval -EINVAL if an error occurs.
 */
static int run_test() {
	int i;
	TRACE("\n");
	scheduler_init();
	for (i = 0; i < threads_count; i++) {
		threads[i] = thread_create(threads_run, stacks[i] + THREAD_STACK_SIZE);
		assert(threads[i] != NULL);
		threads[i]->priority = i+1;
		thread_start(threads[i]);
	}

	TRACE("\nBefore start\n");
	scheduler_start();

	return 0;
}
