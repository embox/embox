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

static struct thread idle = { .priority = THREAD_PRIORITY_MIN, .sched_list =
		LIST_HEAD_INIT(idle.sched_list), };

EMBOX_TEST_SUITE("priority_based scheduling algorithm tests");

TEST_SETUP(setup);

TEST_CASE("initially _scheduler_current should return an idle thread") {
	test_assert_equal(sched_policy_current(), &idle);
}

TEST_CASE("_scheduler_next should return an idle when there is no more thread") {
	test_assert_equal(sched_policy_switch(sched_policy_current()), &idle);
}

TEST_CASE("current thread shouldn't be changed until _scheduler_next is called") {
	struct thread thread = { .priority = 1 };
	sched_policy_add(&thread);
	test_assert_equal(sched_policy_current(), &idle);
}

TEST_CASE("_scheduler_next should return added thread after preemption") {
	struct thread thread = { .priority = 1 };
	sched_policy_add(&thread);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &thread);
}

TEST_CASE("_scheduler_next should switch to the most priority thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };

	sched_policy_add(&bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);

	sched_policy_add(&fg);
	test_assert_equal(sched_policy_current(), &bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
}

TEST_CASE("Adding and subsequent removing a background thread shouldn't"
		"change the behavior of _scheduler_next") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	sched_policy_add(&bg);
	sched_policy_add(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
	sched_policy_remove(&bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
}

TEST_CASE("_scheduler_next should switch back to a background after removing "
		"a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	sched_policy_add(&bg);
	sched_policy_add(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &fg);
	sched_policy_remove(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);
}

TEST_CASE("_scheduler_next should return background thread after serial adding "
		"and removing a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	sched_policy_add(&bg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);
	sched_policy_add(&fg);
	sched_policy_remove(&fg);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &bg);
}

TEST_CASE("removing a thread with _scheduler_remove shouldn't affect on rest "
		"threads with the same priority") {
	struct thread even = { .priority = 127 }, odd = { .priority = 127 };
	sched_policy_add(&even);
	sched_policy_add(&odd);
	test_assert_equal(sched_policy_current(), &idle);
	sched_policy_remove(&even);
	test_assert_equal(sched_policy_switch(sched_policy_current()), &odd);
}

TEST_CASE("sched_policy_add should return true after adding thread with highest "
		"priority") {
	struct thread t = { .priority = 127 };
	test_assert_true(sched_policy_add(&t));
}

TEST_CASE("sched_policy_add should return false then current thread has "
		"a higher priority than added thread priority") {
	struct thread first = { .priority = 127 }, second = { .priority = 0};
	test_assert_true(sched_policy_add(&second));
	sched_policy_switch(sched_policy_current());
	test_assert_false(sched_policy_add(&first));
}

TEST_CASE("sched_policy_add should return true after adding thread with highest "
		"priority before switch") {
	struct thread first = { .priority = 127 }, second = { .priority = 0};
	test_assert_true(sched_policy_add(&first));
	test_assert_true(sched_policy_add(&second));
}

TEST_CASE("sched_policy_add should return false after adding thread with "
		"THREAD_MIN_PRIORITY") {
	struct thread t = { .priority = 255 };
	test_assert_false(sched_policy_add(&t));
}

TEST_CASE("sched_policy_remove should return false after removing not current "
		"thread") {
	struct thread first = { .priority = 127 }, second = { .priority = 255};
	sched_policy_add(&first);
	sched_policy_add(&second);
	sched_policy_switch(sched_policy_current());
	test_assert_false(sched_policy_remove(&second));
}

TEST_CASE("sched_policy_remove should return true after removing current "
		"thread") {
	struct thread first = { .priority = 127 };
	sched_policy_add(&first);
	sched_policy_switch(sched_policy_current());
	test_assert_true(sched_policy_remove(&first));
}

TEST_CASE("sched_policy_remove should return true after removing current "
		"thread") {
	struct thread first = { .priority = 127 }, second = { .priority = 127 };
	sched_policy_add(&first);
	sched_policy_add(&second);
	sched_policy_switch(sched_policy_current());
	test_assert_true(sched_policy_remove(&first));
}

static int setup(void) {
	sched_policy_init();
	INIT_LIST_HEAD(&idle.sched_list);
	sched_policy_add(&idle);
	return 0;
}
