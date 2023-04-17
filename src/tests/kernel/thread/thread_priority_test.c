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
#include <kernel/task/kernel_task.h>
#include <framework/mod/options.h>

#include <util/err.h>
#include <util/log.h>

EMBOX_TEST_SUITE("test for different priority threads");

#define THREADS_QUANTITY OPTION_GET(NUMBER, threads_quantity)
#define __str(x) #x
#define str(x) __str(x)

static void *thread_run(void *arg) {
	ksleep(2 * 1000);
	return 0;
}

TEST_CASE("Create " str(THREADS_QUANTITY) " threads with"
		" different priority") {
	void *res[THREADS_QUANTITY];
	struct thread *t[THREADS_QUANTITY];
	int ref_priority = thread_self()->schedee.priority.current_priority;

	log_debug("Main process priority %d", ref_priority);

	for (int i = 0; i < THREADS_QUANTITY; i++) {
		log_debug("Thread create %d",i);
		t[i] = thread_create( THREAD_FLAG_SUSPENDED,
				thread_run, (void *) (uintptr_t) i);
		test_assert_zero(err(t[i]));
		test_assert_zero(schedee_priority_set(&t[i]->schedee, ref_priority-(i+1)));
		test_assert_zero(thread_launch(t[i]));
	}
	for (int i = 0; i < THREADS_QUANTITY; i++) {
		test_assert_zero(thread_join(t[i],&res[i]));
		log_debug("Thread joined %d",i);
	}
}
