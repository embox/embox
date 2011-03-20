/**
 * @file
 * @brief Tests priority_based scheduling algorithm.
 *
 * @date 20.03.11
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/scheduler_base.h>

static struct thread idle = { .priority = THREAD_PRIORITY_MIN, .sched_list =
		LIST_HEAD_INIT(idle.sched_list), };

EMBOX_TEST_SUITE("priority_based scheduling algorithm tests");

TEST_CASE("_scheduler_init should make the current idle") {
	_scheduler_init();
	_scheduler_add(&idle);
	test_assert_equal(_scheduler_current(), &idle);
}

TEST_CASE("_scheduler_next should return idle after initialization") {
	_scheduler_init();
	_scheduler_add(&idle);
	test_assert_equal(_scheduler_next(_scheduler_current()), &idle);
}

TEST_CASE("_scheduler_add should add thread to the right place") {
	struct thread thread = { .priority = 1 };
	_scheduler_init();
	_scheduler_add(&idle);
	_scheduler_add(&thread);
	test_assert_equal(_scheduler_current(), &idle);
	test_assert_equal(_scheduler_next(_scheduler_current()), &thread);
}

TEST_CASE("_scheduler_next should return added thread after preemption") {
	struct thread thread = { .priority = 1 };
	_scheduler_init();
	_scheduler_add(&idle);
	_scheduler_add(&thread);
	test_assert_equal(_scheduler_next(_scheduler_current()), &thread);
}

TEST_CASE("_scheduler_next should return second") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	_scheduler_init();
	_scheduler_add(&idle);

	_scheduler_add(&bg);
	test_assert_equal(_scheduler_current(), &idle);
	test_assert_equal(_scheduler_next(_scheduler_current()), &bg);

	_scheduler_add(&fg);
	test_assert_equal(_scheduler_current(), &bg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &fg);
}

TEST_CASE("_sceduler_remove should delete all links on removed thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	_scheduler_init();
	_scheduler_add(&idle);
	_scheduler_add(&bg);
	_scheduler_add(&fg);
	_scheduler_remove(&bg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &fg);
}

#if 0
TEST_CASE("_scheduler_next should return added thread after removing more priority thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	_scheduler_init();
	_scheduler_add(&idle);
	_scheduler_add(&bg);
	_scheduler_add(&fg);
	test_assert_equal(_scheduler_current(), &idle);
	_scheduler_remove(&fg);
	test_assert_equal(_scheduler_next(_scheduler_current()), &fg);

}

TEST_CASE("_scheduler_current and _scheduler_next should return idle after initialization") {
	idle->priority = 0;
	_scheduler_init();
	test_assert_equal(_scheduler_current(), idle);
	test_assert_equal(_scheduler_next(_scheduler_current()), idle);
}
#endif
