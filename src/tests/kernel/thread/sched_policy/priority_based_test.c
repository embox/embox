/**
 * @file
 * @brief Tests priority_based scheduling algorithm.
 *
 * @date 20.03.11
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/thread/sched_policy.h>
#include <kernel/thread/api.h>

static struct thread idle = { .priority = THREAD_PRIORITY_MIN };

static struct thread current = { .priority = THREAD_PRIORITY_MAX };

EMBOX_TEST_SUITE("priority_based scheduling algorithm tests");

TEST_SETUP(setup);

TEST_CASE("initially sched_policy_current should return a current thread") {
	test_assert_equal(sched_policy_current(), &current);
}

TEST_CASE("sched_policy_switch should return a current when there is no more "
		"thread") {
	test_assert_equal(sched_policy_switch(sched_policy_current()), &current);
}

TEST_CASE("current thread shouldn't be changed until sched_policy_switch "
		"is called") {
	struct thread thread = { .priority = 1 };
	sched_policy_start(&thread);
	test_assert_equal(sched_policy_current(), &current);
}

TEST_CASE("sched_policy_switch should return added thread after preemption") {
	struct thread thread = { .priority = 1 };
	sched_policy_start(&thread);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &thread);
}

TEST_CASE("sched_policy_switch should switch to the most priority thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };

	sched_policy_start(&bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);

	sched_policy_start(&fg);
	test_assert_equal(sched_policy_current(), &bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
}

TEST_CASE("Adding and subsequent removing a background thread shouldn't"
		"change the behavior of sched_policy_switch") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	sched_policy_start(&bg);
	sched_policy_start(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
	sched_policy_stop(&bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
}

TEST_CASE("sched_policy_switch should switch back to a background after removing "
		"a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	sched_policy_start(&bg);
	sched_policy_start(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
	sched_policy_stop(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);
}

TEST_CASE("sched_policy_switch should return background thread after serial adding "
		"and removing a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	sched_policy_start(&bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);
	sched_policy_start(&fg);
	sched_policy_stop(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);
}

TEST_CASE("removing a thread with _scheduler_remove shouldn't affect on rest "
		"threads with the same priority") {
	struct thread even = { .priority = 127 }, odd = { .priority = 127 };
	sched_policy_start(&even);
	sched_policy_start(&odd);
	test_assert_equal(sched_policy_current(), &idle);
	sched_policy_stop(&even);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &odd);
}

TEST_CASE("sched_policy_start should return true after adding thread with highest "
		"priority") {
	struct thread t = { .priority = 127 };
	test_assert_true(sched_policy_start(&t));
}

TEST_CASE("sched_policy_start should return false then current thread has "
		"a higher priority than added thread priority") {
	struct thread first = { .priority = 127 }, second = { .priority = 0 };
	test_assert_true(sched_policy_start(&second));
	sched_policy_switch(sched_policy_current());
	test_assert_false(sched_policy_start(&first));
}

TEST_CASE("sched_policy_start should return true after adding thread with highest "
		"priority before switch") {
	struct thread first = { .priority = 127 }, second = { .priority = 0 };
	test_assert_true(sched_policy_start(&first));
	test_assert_true(sched_policy_start(&second));
}

TEST_CASE("sched_policy_start should return false after adding thread with "
		"THREAD_MIN_PRIORITY") {
	struct thread t = { .priority = 255 };
	test_assert_false(sched_policy_start(&t));
}

TEST_CASE("sched_policy_stop should return false after removing not current "
		"thread") {
	struct thread first = { .priority = 127 }, second = { .priority = 255 };
	sched_policy_start(&first);
	sched_policy_start(&second);
	sched_policy_switch(sched_policy_current());
	test_assert_false(sched_policy_stop(&second));
}

TEST_CASE("sched_policy_stop should return true after removing current "
		"thread") {
	struct thread first = { .priority = 127 };
	sched_policy_start(&first);
	sched_policy_switch(sched_policy_current());
	test_assert_true(sched_policy_stop(&first));
}

TEST_CASE("sched_policy_stop should return true after removing current "
		"thread") {
	struct thread first = { .priority = 127 }, second = { .priority = 127 };
	sched_policy_start(&first);
	sched_policy_start(&second);
	sched_policy_switch(sched_policy_current());
	test_assert_true(sched_policy_stop(&first));
}

static int setup(void) {
	INIT_LIST_HEAD(&idle.sched_list);
	INIT_LIST_HEAD(&current.sched_list);

	sched_policy_init(&current, &idle);

	return 0;
}
