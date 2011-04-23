/**
 * @file
 * @brief TODO documentation for state_test.c -- Alina Kramar
 *
 * @date 03.04.2011
 * @author Alina Kramar
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <kernel/thread/state.h>

EMBOX_TEST_SUITE("thread state tests");

TEST_SETUP(setup);

static thread_state_t s;

TEST_CASE("single thread_state_do_sleep") {
	s = thread_state_do_sleep(s);

	test_assert_true(thread_state_sleeping(s));
	test_assert_true(thread_state_blocked(s));

	test_assert_false(thread_state_running(s));
	test_assert_false(thread_state_suspended(s));
}

TEST_CASE("thread_state_do_wake after thread_state_do_sleep") {
	s = thread_state_do_sleep(s);

	test_assert_true(thread_state_sleeping(s));

	s = thread_state_do_wake(s);

	test_assert_true(thread_state_running(s));

	test_assert_false(thread_state_blocked(s));
	test_assert_false(thread_state_suspended(s));
	test_assert_false(thread_state_sleeping(s));
}

TEST_CASE("single thread_state_do_suspend") {
	s = thread_state_do_suspend(s);

	test_assert_true(thread_state_suspended(s));
	test_assert_true(thread_state_blocked(s));

	test_assert_false(thread_state_running(s));
	test_assert_false(thread_state_sleeping(s));
}

TEST_CASE("thread_state_do_resume after thread_state_do_suspend") {
	s = thread_state_do_suspend(s);

	test_assert_true(thread_state_suspended(s));

	s = thread_state_do_resume(s);

	test_assert_true(thread_state_running(s));

	test_assert_false(thread_state_blocked(s));
	test_assert_false(thread_state_suspended(s));
	test_assert_false(thread_state_sleeping(s));
}

static int setup(void) {
	s = thread_state_init();
	return 0;
}
