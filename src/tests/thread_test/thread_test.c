/**
 * @file
 * @brief Tests threads.
 *
 * @date 16.04.2010
 * @author Dmitry Avdyukhin
 */

#include <embox/test.h>
#include <hal/thread.h>

EMBOX_TEST(run);

#define THREAD_STACK_SIZE 1000
static int array[THREAD_STACK_SIZE];

static int run() {
	idle_thread = thread_create(idle_run, array, (size_t)sizeof(array));
	return 0;
}
