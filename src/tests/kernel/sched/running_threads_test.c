/**
 * @file
 *
 * @date 28.03.2017
 * @author Alex Kalmuk
 */

#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/printk.h>
#include <hal/reg.h>
#include <kernel/time/ktime.h>

EMBOX_TEST_SUITE("Two running threads sheduling test");

static void *t1_run(void *arg);
static void *t2_run(void *arg);

static int flag1 = 0, flag2 = 0;

TEST_CASE("two_running_threads_test") {
	struct thread *t1, *t2;

	t1 = thread_create(THREAD_FLAG_SUSPENDED, t1_run, NULL);
	t2 = thread_create(THREAD_FLAG_SUSPENDED, t2_run, NULL);

	thread_launch(t1);
	test_assert_zero(thread_detach(t1));

	thread_launch(t2);
	test_assert_zero(thread_detach(t2));

	ksleep(2000);

	thread_terminate(t1);
	thread_terminate(t2);
}

static void *t1_run(void *arg) {
	int i = 0;

	flag1 = 1;
	while (i++ < 10000000)
		;
	test_assert(flag2 != 0);

	return NULL;
}

static void *t2_run(void *arg) {
	int i = 0;

	flag2 = 1;
	while (i++ < 10000000)
		;
	test_assert(flag1 != 0);

	return NULL;
}
