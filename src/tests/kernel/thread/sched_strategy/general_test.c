/**
 * @file
 * @brief TODO documentation for general_test.c -- Bulychev Anton
 *
 * @date Apr 2, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>

#include <kernel/thread/state.h>
#include <kernel/thread/sched_priority.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/api.h>

struct runq runq;
struct thread idle, current;

EMBOX_TEST_SUITE("Scheduler strategy general test");

TEST_SETUP(setup);

static void make_switch(void);
static void thread_initialize(struct thread* thread, __thread_priority_t priority, __thread_state_t state);

TEST_CASE("Initialization") {
	test_assert_true(thread_state_running(current.state));
	test_assert_true(thread_state_running(idle.state));
	test_assert_true(runq_current(&runq) == &current);
}

TEST_CASE("Resume") {
	struct thread new;
	thread_initialize(&new, THREAD_PRIORITY_NORMAL, __THREAD_STATE_SUSPENDED);

	runq_resume(&runq, &new);
	test_assert_true(thread_state_running(new.state));
	make_switch();
}

TEST_CASE("Suspend/resume running 1") {
	test_assert_not_zero(runq_suspend(&runq, &current));
	test_assert_true(thread_state_suspended(current.state));
	make_switch();
	runq_resume(&runq, &current);
	test_assert_true(thread_state_running(current.state));
	make_switch();
}

TEST_CASE("Suspend/resume running 2") {
	test_assert_not_zero(runq_suspend(&runq, &current));
	test_assert_true(thread_state_suspended(current.state));
	runq_resume(&runq, &current);
	test_assert_true(thread_state_running(current.state));
	make_switch();
}

TEST_CASE("Sleep/wake running 1") {
	struct sleepq sleepq;
	sleepq_init(&sleepq);

	runq_sleep(&runq, &sleepq);
	test_assert_true(thread_state_sleeping(current.state));
	make_switch();
	sleepq_wake(&runq, &sleepq, 1);
	test_assert_true(thread_state_running(current.state));
	make_switch();
}

TEST_CASE("Sleep/wake running 2") {
	struct sleepq sleepq;
	sleepq_init(&sleepq);

	runq_sleep(&runq, &sleepq);
	test_assert_true(thread_state_sleeping(current.state));
	sleepq_wake(&runq, &sleepq, 1);
	test_assert_true(thread_state_running(current.state));
	make_switch();
}

TEST_CASE("Suspend/resume sleeping") {
	struct sleepq sleepq;
	sleepq_init(&sleepq);

	runq_sleep(&runq, &sleepq);
	test_assert_true(thread_state_sleeping(current.state));
	make_switch();

	sleepq_suspend(&sleepq, &current);
	test_assert_true(thread_state_suspended(current.state));

	sleepq_resume(&sleepq, &current);
	test_assert_true(!thread_state_suspended(current.state));

	sleepq_wake(&runq, &sleepq, 1);
	test_assert_true(thread_state_running(current.state));
	make_switch();
}

TEST_CASE("Wake resumed thread") {
	struct sleepq sleepq;
	sleepq_init(&sleepq);

	runq_sleep(&runq, &sleepq);
	test_assert_true(thread_state_sleeping(current.state));

	sleepq_wake_resumed_thread(&runq, &sleepq, &current);
	test_assert_true(thread_state_running(current.state));
	make_switch();
}

TEST_CASE("Wake suspended thread") {
	struct sleepq sleepq;
	sleepq_init(&sleepq);

	runq_sleep(&runq, &sleepq);
	test_assert_true(thread_state_sleeping(current.state));

	sleepq_suspend(&sleepq, &current);
	test_assert_true(thread_state_suspended(current.state));

	sleepq_wake_suspended_thread(&sleepq, &current);
	test_assert_true(thread_state_suspended(current.state));
	make_switch();
}

TEST_CASE("Change priority of running") {
	runq_change_priority(&runq, &current, THREAD_PRIORITY_MAX);
	test_assert_true(current.priority == THREAD_PRIORITY_MAX);
}

TEST_CASE("Change priority of sleeping") {
	struct sleepq sleepq;
	sleepq_init(&sleepq);

	runq_sleep(&runq, &sleepq);
	test_assert_true(thread_state_sleeping(current.state));
	make_switch();

	sleepq_change_priority(&sleepq, &current, THREAD_PRIORITY_MAX);
	test_assert_true(current.priority == THREAD_PRIORITY_MAX);

	sleepq_wake(&runq, &sleepq, 1);
	test_assert_true(thread_state_running(current.state));
	make_switch();
}

static void make_switch(void) {
	struct thread *prev = runq_current(&runq);
	if (runq_switch(&runq)) {
		test_assert_true(runq_current(&runq) != prev);
	}
}

static void thread_initialize(struct thread* thread, __thread_priority_t priority, __thread_state_t state) {
	sched_strategy_init(&thread->sched);
	thread->priority = priority;
	thread->state = state;
}


static int setup(void) {
	thread_initialize(&idle, THREAD_PRIORITY_MIN, __THREAD_STATE_SUSPENDED);
	thread_initialize(&current, THREAD_PRIORITY_NORMAL, __THREAD_STATE_SUSPENDED);
	runq_init(&runq, &current, &idle);
	return 0;
}
