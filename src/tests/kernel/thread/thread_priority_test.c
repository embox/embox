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

EMBOX_TEST_SUITE("test for different priority threads");

#define THREADS_QUANTITY  0x100

static void *thread_run(void *arg) {
	ksleep(100 * 1000);
	return 0;
}

TEST_CASE("Create 256 threads with different priority") {
	for (int i = 0; i < THREADS_QUANTITY; i++) {
		struct thread *t;
		test_assert_zero(thread_create(&t,
				0, thread_run, (void *) i));
		test_assert_zero(thread_set_priority(t, i));
		test_assert_zero(thread_detach(t));
	}
}
