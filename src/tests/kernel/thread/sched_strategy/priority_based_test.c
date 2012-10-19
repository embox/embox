/**
 * @file
 * @brief Tests priority_based scheduling algorithm.
 *
 * @date 20.03.11
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/thread/api.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

struct runq rq;
static struct thread current, idle;

EMBOX_TEST_SUITE("priority_based scheduling algorithm tests");

TEST_SETUP(setup);

static void setup_thread(struct thread *t, thread_priority_t prio) {
	t->state = thread_state_init();
	t->priority = prio;
	sched_strategy_init(&t->sched);
}

TEST_CASE("initially runq_current should return a current thread") {
	test_assert_equal(runq_current(&rq), &current);
}

TEST_CASE("runq_current should return a current when there is no more "
		"thread") {
	test_assert_zero(runq_switch(&rq));
}

TEST_CASE("current thread shouldn't be changed until runq_switch "
		"is called") {
	struct thread thread;
	setup_thread(&thread, THREAD_PRIORITY_MAX);
	runq_start(&rq, &thread);
	test_assert_equal(runq_current(&rq), &current);
}

TEST_CASE("runq_switch should return true after preemption") {
	struct thread thread;
	setup_thread(&thread, THREAD_PRIORITY_MAX);
	runq_start(&rq, &thread);
	test_assert_not_zero(runq_switch(&rq));
}

#if 0
TEST_CASE("runq_switch should switch to the most priority thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };

	runq_start(&bg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &bg);

	runq_start(&fg);
	test_assert_equal(runq_current(&rq), &bg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &fg);
}

TEST_CASE("Adding and subsequent removing a background thread shouldn't"
		"change the behavior of sched_policy_switch") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	runq_start(&bg);
	runq_start(&fg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &fg);
	sched_policy_stop(&bg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &fg);
}

TEST_CASE("sched_policy_switch should switch back to a background after removing "
		"a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	runq_start(&bg);
	runq_start(&fg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &fg);
	sched_policy_stop(&fg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &bg);
}

TEST_CASE("sched_policy_switch should return background thread after serial adding "
		"and removing a foreground thread") {
	struct thread bg = { .priority = 127 }, fg = { .priority = 0 };
	runq_start(&bg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &bg);
	runq_start(&fg);
	sched_policy_stop(&fg);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &bg);
}

TEST_CASE("removing a thread with _scheduler_remove shouldn't affect on rest "
		"threads with the same priority") {
	struct thread even = { .priority = 127 }, odd = { .priority = 127 };
	runq_start(&even);
	runq_start(&odd);
	test_assert_equal(runq_current(&rq), &idle);
	sched_policy_stop(&even);
	test_assert_equal(runq_switch(&rq, runq_current(&rq)), &odd);
}

TEST_CASE("sched_policy_start should return true after adding thread with highest "
		"priority") {
	struct thread t = { .priority = 127 };
	test_assert_true(runq_start(&t));
}

TEST_CASE("sched_policy_start should return false then current thread has "
		"a higher priority than added thread priority") {
	struct thread first = { .priority = 127 }, second = { .priority = 0 };
	test_assert_true(runq_start(&second));
	runq_switch(&rq, runq_current(&rq));
	test_assert_false(runq_start(&first));
}

TEST_CASE("sched_policy_start should return true after adding thread with highest "
		"priority before switch") {
	struct thread first = { .priority = 127 }, second = { .priority = 0 };
	test_assert_true(runq_start(&first));
	test_assert_true(runq_start(&second));
}

TEST_CASE("sched_policy_start should return false after adding thread with "
		"THREAD_MIN_PRIORITY") {
	struct thread t = { .priority = 255 };
	test_assert_false(runq_start(&t));
}

TEST_CASE("sched_policy_stop should return false after removing not current "
		"thread") {
	struct thread first = { .priority = 127 }, second = { .priority = 255 };
	runq_start(&first);
	runq_start(&second);
	runq_switch(&rq, runq_current(&rq));
	test_assert_false(sched_policy_stop(&second));
}

TEST_CASE("sched_policy_stop should return true after removing current "
		"thread") {
	struct thread first = { .priority = 127 };
	runq_start(&first);
	runq_switch(&rq, runq_current(&rq));
	test_assert_true(sched_policy_stop(&first));
}

TEST_CASE("sched_policy_stop should return true after removing current "
		"thread") {
	struct thread first = { .priority = 127 }, second = { .priority = 127 };
	runq_start(&first);
	runq_start(&second);
	runq_switch(&rq, runq_current(&rq));
	test_assert_true(sched_policy_stop(&first));
}
#endif

static int setup(void) {
	setup_thread(&current, THREAD_PRIORITY_NORMAL);
	setup_thread(&idle, THREAD_PRIORITY_MIN);

	runq_init(&rq, &current, &idle);

	return 0;
}
