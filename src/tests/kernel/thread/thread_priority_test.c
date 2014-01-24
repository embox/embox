/**
 * @file
 *
 * @brief Testing interface for thread with different priority
 *
 * @date Oct 17, 2012
 * @author: Anton Bondarev
 */

#include <embox/test.h>

#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <kernel/task.h>
#include <util/math.h>

#include <err.h>

EMBOX_TEST_SUITE("test for different priority threads");

#include <module/embox/kernel/thread/core.h>
#define THREADS_TOTAL \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_pool_size)
#define THREADS_USED 2 /* boot & idle */
#define THREADS_FREE (THREADS_TOTAL - THREADS_USED)
#define THREADS_QUANTITY min(THREADS_FREE, 256)

static void *thread_run(void *arg) {
	ksleep(2 * 1000);
	return 0;
}

TEST_CASE("Create maxinum amount of threads with different priority") {
	for (int i = 0; i < THREADS_QUANTITY; i++) {
		struct thread *t;
		t = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
				thread_run, (void *) i);
		test_assert_zero(err(t));
		thread_register(task_kernel_task(), t);
		test_assert_zero(thread_set_priority(t, i));
		test_assert_zero(thread_detach(t));
		test_assert_zero(thread_launch(t));
	}
}
