/**
 * @file
 * @brief Tests priority_based scheduling algorithm.
 *
 * @date 20.03.11
 * @author Alina Kramar
 * @author Eldar Abusalimov
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
	test_assert_false(runq_switch(&rq));
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
	test_assert_true(runq_switch(&rq));
}

TEST_CASE("runq_switch should switch to the most priority thread") {
	struct thread bg, fg;
	setup_thread(&bg, THREAD_PRIORITY_LOW);
	setup_thread(&fg, THREAD_PRIORITY_HIGH);

	test_assert_equal(runq_current(&rq), &current);

	test_assert_false(runq_start(&rq, &bg));

	test_assert_equal(runq_current(&rq), &current);

	test_assert_true(runq_start(&rq, &fg));
	test_assert_true(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &fg);

	test_assert_false(runq_finish(&rq, &bg));
	test_assert_true(runq_finish(&rq, &fg));
	test_assert_true(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &current);
}

TEST_CASE("Adding and subsequent removing a background thread shouldn't"
		"change the behavior of runq_switch") {
	struct thread bg, fg;
	setup_thread(&bg, THREAD_PRIORITY_LOW);
	setup_thread(&fg, THREAD_PRIORITY_HIGH);

	test_assert_true(runq_start(&rq, &fg));
	test_assert_true(runq_switch(&rq));

	test_assert_false(runq_start(&rq, &bg));
	test_assert_equal(runq_current(&rq), &fg);
	test_assert_false(runq_finish(&rq, &bg));
	test_assert_equal(runq_current(&rq), &fg);

	test_assert_true(runq_finish(&rq, &fg));
	test_assert_true(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &current);
}

TEST_CASE("runq_switch should switch back to the current after removing "
		"a thread with higher priority") {
	struct thread fg;
	setup_thread(&fg, THREAD_PRIORITY_HIGH);

	test_assert_equal(runq_current(&rq), &current);

	test_assert_true(runq_start(&rq, &fg));
	test_assert_true(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &fg);

	test_assert_true(runq_finish(&rq, &fg));
	test_assert_true(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &current);
}

TEST_CASE("adding and consequent removing of a foreground thread without "
		"calling runq_switch shouldn't affect the result of runq_current") {
	struct thread fg;
	setup_thread(&fg, THREAD_PRIORITY_HIGH);

	test_assert_equal(runq_current(&rq), &current);

	test_assert_true(runq_start(&rq, &fg));
	test_assert_equal(runq_current(&rq), &current);

	test_assert_false(runq_finish(&rq, &fg));
	test_assert_equal(runq_current(&rq), &current);

	test_assert_false(runq_switch(&rq));
	test_assert_equal(runq_current(&rq), &current);
}

TEST_CASE("removing a thread shouldn't affect the rest threads "
		"with the same priority") {
	struct thread even, odd;
	setup_thread(&even, THREAD_PRIORITY_HIGH);
	setup_thread(&odd, THREAD_PRIORITY_HIGH);

	test_assert_equal(runq_current(&rq), &current);

	test_assert_true(runq_start(&rq, &even));
	test_assert_true(runq_start(&rq, &odd));

	test_assert_equal(runq_current(&rq), &current);

	test_assert_true(runq_switch(&rq));

	test_assert_true(runq_finish(&rq, &even));
	test_assert_true(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &odd);

	test_assert_true(runq_finish(&rq, &odd));
	test_assert_true(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &current);

}

TEST_CASE("runq_start should return true after adding thread with higher "
		"priority") {
	struct thread fg;
	setup_thread(&fg, THREAD_PRIORITY_HIGH);

	test_assert_equal(runq_current(&rq), &current);

	test_assert_true(runq_start(&rq, &fg));
	test_assert_false(runq_finish(&rq, &fg));
	test_assert_false(runq_switch(&rq));

	test_assert_equal(runq_current(&rq), &current);
}

TEST_CASE("runq_start should return false after adding thread with lower "
		"priority") {
	struct thread bg;
	setup_thread(&bg, THREAD_PRIORITY_LOW);

	test_assert_equal(runq_current(&rq), &current);

	test_assert_false(runq_start(&rq, &bg));
	test_assert_false(runq_finish(&rq, &bg));

	test_assert_equal(runq_current(&rq), &current);
}

static int setup(void) {
	setup_thread(&current, THREAD_PRIORITY_NORMAL);
	setup_thread(&idle, THREAD_PRIORITY_MIN);

	runq_init(&rq, &current, &idle);

	return 0;
}
