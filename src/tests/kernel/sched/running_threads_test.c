/**
 * @file
 *
 * @date 28.03.2017
 * @author Alex Kalmuk
 */

#include <embox/test.h>

#include <kernel/thread.h>

EMBOX_TEST_SUITE("Two running threads scheduling test");

static void *t1_run(void *arg);
static void *t2_run(void *arg);

struct thread_args {
	int flag1;
	int flag2;
};

TEST_CASE("two_running_threads_test") {
	struct thread *t1, *t2;
	struct thread_args volatile args;

	args.flag1 = 0;
	args.flag2 = 0;

	t1 = thread_create(0, t1_run, (void *)&args);
	t2 = thread_create(0, t2_run, (void *)&args);

	test_assert_zero(thread_join(t1, NULL));
	test_assert_zero(thread_join(t2, NULL));
}

static void *t1_run(void *arg) {
	struct thread_args *volatile args = arg;
	volatile int i = 0;

	args->flag1 = 1;
	while (i++ < 10000000)
		;
	test_assert(args->flag2 != 0);

	return NULL;
}

static void *t2_run(void *arg) {
	struct thread_args *volatile args = arg;
	volatile int i = 0;

	args->flag2 = 1;
	while (i++ < 10000000)
		;
	test_assert(args->flag1 != 0);

	return NULL;
}
