/**
 * @file
 *
 * @brief Testing interface for thread with different priority
 *
 * @date 2021.12.27
 * @author: Kevin Peck
 */

#include <embox/test.h>

#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <framework/mod/options.h>
#include <kernel/printk.h>
#include <util/err.h>
#include <util/log.h>

EMBOX_TEST_SUITE("test for joining threads");

#define THREADS_QUANTITY OPTION_GET(NUMBER, threads_quantity)
#define __str(x) #x
#define str(x) __str(x)

static void *thread_run(void *arg) {
	int v = 2;
	for(int i=0; i < 1<<18; i++) {
		// some work
		v *= i;
	};
	return 0;
}

TEST_CASE("Create " str(THREADS_QUANTITY) " threads with"
		" different priority, then join them") {
	struct thread *t[THREADS_QUANTITY];
	void *res[THREADS_QUANTITY];

	for (int i = 0; i < THREADS_QUANTITY; i++) {
		log_debug("Create thread %d",i);
		t[i] = thread_create( THREAD_FLAG_SUSPENDED | THREAD_FLAG_JOINABLE,
				thread_run, (void *) (uintptr_t) i);
		test_assert_zero(err(t[i]));
		test_assert_zero(thread_launch(t[i]));
	}
	for (int i = 0; i < THREADS_QUANTITY; i++) {
		thread_join(t[i],&res[i]);
		log_debug("Join thread %d.",i);
	}
}
