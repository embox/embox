/**
 * @file
 * @brief TODO documentation for core_test.c -- Alina Kramar
 *
 * @date 09.04.2011
 * @author Alina Kramar
 */

#include <errno.h>
#include <embox/test.h>

#include <kernel/thread/api.h>

EMBOX_TEST_SUITE("test for thread API");

static void *foo_run(void *arg) {
	return (void *) ~((unsigned long) arg);
}

TEST_CASE("thread_join should retrieve the result of thread execution") {
	struct thread *foo;
	void *ret;

	test_assert_zero(thread_create(&foo, 0, foo_run, (void *) 42UL));
	test_assert_zero(thread_join(foo, &ret));
	test_assert_equal(ret, (void *) ~42UL);
}

TEST_CASE("thread_create should return EINVAL when thread function is NULL") {
	struct thread *foo;

	test_assert_equal(-EINVAL, thread_create(&foo, 0, NULL, NULL));
}

TEST_CASE("If flag in THREAD_FLAG_DETACHED, thread_create should return NULL and "
		"pointer to struct thread should be not changed") {
	struct thread *bar = (struct thread *) 0xa5a5a5a5;

	test_assert_zero(thread_create(&bar, THREAD_FLAG_DETACHED, foo_run, NULL));
	test_assert_equal(bar, (struct thread *) 0xa5a5a5a5);
}

TEST_CASE("thread_create should return EINVAL after calls with NULL pointer to "
		"thread") {
	test_assert_equal(-EINVAL, thread_create(NULL, 0, foo_run, (void *) 1));
}

TEST_CASE("thread_resume should return EINVAL because thread wasn't suspended") {
	struct thread *foo;

	test_assert_zero(thread_create(&foo, 0, foo_run, NULL));
	test_assert_equal(thread_resume(foo), -EINVAL);
}

TEST_CASE("thread_resume should return 0 because thread was suspended") {
	struct thread *foo;

	test_assert_zero(thread_create(&foo, THREAD_FLAG_SUSPENDED, foo_run, NULL));
	test_assert_zero(thread_resume(foo));
}


