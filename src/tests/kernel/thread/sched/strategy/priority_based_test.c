/**
 * @file
 * @brief Tests priority_based scheduling algorithm.
 *
 * @date 20.03.11
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/thread/sched_logic.h>
#include <kernel/thread/api.h>

static struct thread idle = { .priority = THREAD_PRIORITY_MIN, .sched_list =
		LIST_HEAD_INIT(idle.sched_list), };

EMBOX_TEST_SUITE("priority_based scheduling algorithm tests");

TEST_SETUP(setup);

TEST_CASE("initially _scheduler_current should return an idle thread") {
	test_assert_equal(_scheduler_current(), &idle);
}

TEST_CASE("_scheduler_next should return an idle when there is no more thread") {
	test_assert_equal(_scheduler_next(_scheduler_current()), &idle);
}

TEST_CASE("current thread shouldn't be changed until _scheduler_next is called") {
	struct thread thread = { .priority = 1 };
	_scheduler_add(&thread);
	test_assert_equal(_scheduler_current(), &idle);
}

TEST_CASE("_scheduler_next should return added thread after preemption") {
	struct thread thread = { .priority = 1 };
	_scheduler_add(&thread);
	test_assert_equal(_scheduler_next(_scheduler_current()), &thread);
}

TEST_CASE("_scheduler_next should switch to the most priority thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };

	_scheduler_add(&bg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &bg);

	_scheduler_add(&fg);
	test_assert_equal(_scheduler_current(), &bg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &fg);
}

TEST_CASE("Adding and subsequent removing a background thread shouldn't"
		"change the behavior of _scheduler_next") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	_scheduler_add(&bg);
	_scheduler_add(&fg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &fg);
	_scheduler_remove(&bg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &fg);
}

TEST_CASE("_scheduler_next should switch back to a background after removing "
		"a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	_scheduler_add(&bg);
	_scheduler_add(&fg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &fg);
	_scheduler_remove(&fg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &bg);
}

TEST_CASE("_scheduler_next should return background thread after serial adding "
		"and removing a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	_scheduler_add(&bg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &bg);
	_scheduler_add(&fg);
	_scheduler_remove(&fg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &bg);
}

TEST_CASE("removing a thread with _scheduler_remove shouldn't affect on rest "
		"threads with the same priority") {
	struct thread even = { .priority = 127 }, odd = { .priority = 127 };
	_scheduler_add(&even);
	_scheduler_add(&odd);
	test_assert_equal(_scheduler_current(), &idle);
	_scheduler_remove(&even);
	test_assert_equal(_scheduler_next(_scheduler_current()), &odd);
}

static int setup(void) {
	_scheduler_init();
	INIT_LIST_HEAD(&idle.sched_list);
	_scheduler_add(&idle);
	return 0;
}
