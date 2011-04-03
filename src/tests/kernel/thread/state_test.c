/**
 * @file
 * @brief TODO documentation for state_test.c -- Alina Kramar
 *
 * @date 3.04.2011
 * @author Alina Kramar
 */

#include <embox/test.h>

#include __impl_x(kernel/thread/state.h)

EMBOX_TEST_SUITE("thread state machine tests");

#define STATE(state) \
	THREAD_STATE_ ## state

#define ACTION(action) \
	THREAD_STATE_ACTION_ ## action

#define assert_transition(from, action, to) \
	test_assert_equal(thread_state_transition(from, action), to);

#define assert_transition_invalid(from, action) \
	test_assert_zero(thread_state_transition(from, action));

TEST_CASE("Valid states are not zeroes") {
	test_assert_not_zero(STATE(WAIT));
	test_assert_not_zero(STATE(SUSP));
	test_assert_not_zero(STATE(WAIT_SUSP));
	test_assert_not_zero(STATE(RUNNING));
	test_assert_not_zero(STATE(TERMINATE));
}

TEST_CASE("Valid actions for TERMINATE state") {
	assert_transition(STATE(TERMINATE), ACTION(START), STATE(RUNNING));
	assert_transition(STATE(TERMINATE), ACTION(STOP), STATE(TERMINATE));
}

TEST_CASE("Invalid actions for TERMINATE state") {
	assert_transition_invalid(STATE(TERMINATE), ACTION(WAKE));
	assert_transition_invalid(STATE(TERMINATE), ACTION(SLEEP));
	assert_transition_invalid(STATE(TERMINATE), ACTION(RESUME));
	assert_transition_invalid(STATE(TERMINATE), ACTION(SUSPEND));
}

TEST_CASE("Valid actions for RUNNING state") {
	assert_transition(STATE(RUNNING), ACTION(STOP), STATE(TERMINATE));
	assert_transition(STATE(RUNNING), ACTION(SUSPEND), STATE(SUSP));
	assert_transition(STATE(RUNNING), ACTION(SLEEP), STATE(WAIT));
}

TEST_CASE("Invalid actions for RUNNING state") {
	assert_transition_invalid(STATE(RUNNING), ACTION(WAKE));
	assert_transition_invalid(STATE(RUNNING), ACTION(START));
	assert_transition_invalid(STATE(RUNNING), ACTION(RESUME));
}

TEST_CASE("Valid actions for SLEEPING state") {
	assert_transition(STATE(WAIT), ACTION(WAKE), STATE(RUNNING));
	assert_transition(STATE(WAIT), ACTION(SUSPEND), STATE(WAIT_SUSP));
	assert_transition(STATE(WAIT), ACTION(STOP), STATE(TERMINATE));
}

TEST_CASE("Invalid actions for SLEEPING state") {
	assert_transition_invalid(STATE(WAIT), ACTION(SLEEP));
	assert_transition_invalid(STATE(WAIT), ACTION(RESUME));
	assert_transition_invalid(STATE(WAIT), ACTION(START));
}

TEST_CASE("Valid actions for SLEEPING_SUSPENDED state") {
	assert_transition(STATE(WAIT_SUSP), ACTION(STOP), STATE(TERMINATE));
	assert_transition(STATE(WAIT_SUSP), ACTION(WAKE), STATE(SUSP));
	assert_transition(STATE(WAIT_SUSP), ACTION(RESUME), STATE(WAIT));
	assert_transition(STATE(WAIT_SUSP), ACTION(SUSPEND), STATE(WAIT_SUSP));
}

TEST_CASE("Invalid actions for SLEEPING_SUSPENDED state") {
	assert_transition_invalid(STATE(WAIT_SUSP), ACTION(START));
	assert_transition_invalid(STATE(WAIT_SUSP), ACTION(SLEEP));
}

TEST_CASE("Valid actions for SUSPENDED state") {
	assert_transition(STATE(SUSP), ACTION(STOP), STATE(TERMINATE));
	assert_transition(STATE(SUSP), ACTION(SUSPEND), STATE(SUSP));
	assert_transition(STATE(SUSP), ACTION(RESUME), STATE(RUNNING));
}

TEST_CASE("Invalid actions for SUSPENDED state") {
	assert_transition_invalid(STATE(SUSP), ACTION(WAKE));
	assert_transition_invalid(STATE(SUSP), ACTION(SLEEP));
	assert_transition_invalid(STATE(SUSP), ACTION(START));
}
