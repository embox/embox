/**
 * @file
 * @brief Tests thread API.
 *
 * @details This test set testing thread API (threads creation, threads join,
 *      threads suspend/resume and detaching)
 *
 * @date 09.04.11
 * @author Alina Kramar
 */

#include <errno.h>
#include <unistd.h>
#include <embox/test.h>

#include <kernel/thread.h>
#include <util/err.h>

EMBOX_TEST_SUITE("test for thread API");

static void *arg_invert_run(void *arg) {
	return (void *) ~((unsigned long) arg);
}
#if 0
TEST_CASE("thread_create with default flags should return -EINVAL if the "
		"first argument is NULL") {
	struct thread *t;

	t = thread_create(0, arg_invert_run, (void *) 1);

	test_assert_equal(ptr2err(t), -EINVAL);
}
#endif

TEST_CASE("thread_create should return -EINVAL if thread function is NULL") {
	struct thread *t;

	t = thread_create(0, NULL, NULL);

	test_assert_equal(ptr2err(t), -EINVAL);
}

#if 0
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
#endif

TEST_CASE("thread_join should retrieve the result of thread execution") {
	void *ret;
	struct thread *t;

	t = thread_create(0, arg_invert_run, (void *) 42UL);
	test_assert_zero(ptr2err(t));

	test_assert_zero(sleep(1));

	test_assert_zero(thread_join(t, &ret));
	test_assert_equal(ret, (void *) ~42UL);
}

TEST_CASE("thread_launch should return 0 if the thread was created with "
		"THREAD_FLAG_SUSPENDED flag") {
	struct thread *t;

	t = thread_create(THREAD_FLAG_SUSPENDED, arg_invert_run, NULL);
	test_assert_zero(ptr2err(t));

	test_assert_zero(thread_launch(t));
	test_assert_zero(thread_detach(t));
}

TEST_CASE("thread_launch should return an error if the thread hasn't been "
		"initially suspended") {
	struct thread *t;

	t = thread_create(0, arg_invert_run, NULL);
	test_assert_zero(ptr2err(t));

	test_assert_zero(sleep(1));

	test_assert_not_zero(thread_launch(t));
	test_assert_zero(thread_detach(t));
}
