/**
 * @file lthread_test.c
 * @brief simple test for lthreads
 *
 * @author Andrey Kokorev
 * @date    16.02.2014
 */
#include <util/err.h>
#include <embox/test.h>
#include <kernel/sched.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>
#include <kernel/time/ktime.h>

#define LTHREAD_QUANTITY OPTION_GET(NUMBER, lthreads_quantity)

EMBOX_TEST_SUITE("test for lthread API");

static volatile int done = 0;

static int run1(struct lthread *self) {
	test_emit('a');
	done++;
	return 0;
}

TEST_CASE("Launch simple lthread") {
	struct lthread lt;

	done = 0;

	lthread_init(&lt, run1);
	lthread_launch(&lt);

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}

	lthread_reset(&lt);

	test_assert_emitted("a");
}

static int run_resched(struct lthread *self) {
	ksleep(0);
	done = 1;
	return 0;
}

TEST_CASE("Call sched from lthread") {
	struct lthread lt;

	done = 0;

	lthread_init(&lt, run_resched);
	lthread_launch(&lt);

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}

	lthread_reset(&lt);
}

static int run2(struct lthread *self) {
	done++;
	return 0;
}

TEST_CASE("Create lthreads with different priorities") {
	struct lthread lts[LTHREAD_QUANTITY];
	done = 0;

	for(int i = 0; i < LTHREAD_QUANTITY; i++) {
		lthread_init(&lts[i], run2);
		test_assert_zero(
			schedee_priority_set(&lts[i].schedee, SCHED_PRIORITY_MAX - i)
		);
		lthread_launch(&lts[i]);
	}

	/* deleting occurs onle after finishing */
	for(int i = 0; i < LTHREAD_QUANTITY; i++) {
		lthread_reset(&lts[i]);
	}

	test_assert_equal(done, LTHREAD_QUANTITY);
}

static int run3(struct lthread *self) {
	test_emit('b');
	done++;
	return 0;
}

TEST_CASE("Test executing order") {
	struct lthread lt1, lt2;

	done = 0;

	lthread_init(&lt1, run1);
	schedee_priority_set(&lt1.schedee, SCHED_PRIORITY_MAX - 1);

	lthread_init(&lt2, run3);
	schedee_priority_set(&lt2.schedee, SCHED_PRIORITY_MAX);

	/* prevent scheduling to avoid executing one
	 * before adding another to runq
	 */
	sched_lock();
	{
		lthread_launch(&lt1);
		lthread_launch(&lt2);
	}
	sched_unlock();

	lthread_reset(&lt1);
	lthread_reset(&lt2);

	test_assert_emitted("ba");
}

TEST_CASE("Test several launches") {
	struct lthread lt;

	done = 0;

	lthread_init(&lt, run1);
	schedee_priority_set(&lt.schedee, SCHED_PRIORITY_MAX);

	lthread_launch(&lt);
	ksleep(0);
	lthread_launch(&lt);
	ksleep(0);

	test_assert_equal(2, done);

	lthread_reset(&lt);
}

