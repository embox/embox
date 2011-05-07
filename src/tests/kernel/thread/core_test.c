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

static void *arg_invert_run(void *arg) {
	return (void *) ~((unsigned long) arg);
}

TEST_CASE("thread_create with default flags should return -EINVAL if the "
		"first argument is NULL") {
	test_assert_equal(thread_create(NULL, 0, arg_invert_run, (void *) 1),
			-EINVAL);
}

TEST_CASE("thread_create should return -EINVAL if thread function is NULL") {
	struct thread *foo;

	test_assert_equal(thread_create(&foo, 0, NULL, NULL), -EINVAL);
}

TEST_CASE("thread_create with THREAD_FLAG_DETACHED should return zero and "
		"shouldn't touch the pointer") {
	struct thread *bar = (struct thread *) 0xa5a5a5a5;

	test_assert_zero(
			thread_create(&bar, THREAD_FLAG_DETACHED, arg_invert_run, NULL));
	test_assert_equal(bar, (struct thread *) 0xa5a5a5a5);
}

TEST_CASE("thread_create with THREAD_FLAG_DETACHED should return zero even if "
		"the pointer is NULL") {
	test_assert_zero(
			thread_create(NULL, THREAD_FLAG_DETACHED, arg_invert_run, NULL));
}

TEST_CASE("thread_join should retrieve the result of thread execution") {
	struct thread *foo;
	void *ret;

	test_assert_zero(thread_create(&foo, 0, arg_invert_run, (void *) 42UL));
	test_assert_zero(thread_join(foo, &ret));
	test_assert_equal(ret, (void *) ~42UL);
}

TEST_CASE("thread_resume should return 0 if the thread was created with "
		"THREAD_FLAG_SUSPENDED flag") {
	struct thread *foo;

	test_assert_zero(thread_create(&foo, THREAD_FLAG_SUSPENDED, arg_invert_run, NULL));
	test_assert_zero(thread_resume(foo));
	test_assert_zero(thread_detach(foo));
}

TEST_CASE("thread_resume should return an error if the thread hasn't been "
		"initially suspended") {
	struct thread *foo;

	test_assert_zero(thread_create(&foo, 0, arg_invert_run, NULL));
	test_assert_not_zero(thread_resume(foo));
	test_assert_zero(thread_detach(foo));
}
