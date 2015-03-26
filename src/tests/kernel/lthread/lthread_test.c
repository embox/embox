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
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>
#include <kernel/time/ktime.h>

#define LTHREAD_QUANTITY OPTION_GET(NUMBER, lthreads_quantity)

EMBOX_TEST_SUITE("test for lthread API");

static volatile int done = 0;

static void *run1(void *arg) {
	test_emit('a');
	done++;
	return NULL;
}

TEST_CASE("Launch simple lthread") {
	struct lthread lt;

	done = 0;

	lthread_init(&lt, run1, NULL);
	lthread_launch(&lt);

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}

	lthread_delete(&lt);

	test_assert_emitted("a");
}

static void *run_resched(void *arg) {
	ksleep(0);
	done = 1;
	return NULL;
}

TEST_CASE("Call sched from lthread") {
	struct lthread lt;

	done = 0;

	lthread_init(&lt, run_resched, NULL);
	lthread_launch(&lt);

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}

	lthread_delete(&lt);
}

static void *run2(void *arg) {
	done++;
	return NULL;
}

TEST_CASE("Create lthreads with different priorities") {
	struct lthread lts[LTHREAD_QUANTITY];
	done = 0;

	for(int i = 0; i < LTHREAD_QUANTITY; i++) {
		lthread_init(&lts[i], run2, NULL);
		test_assert_zero(
			lthread_priority_set(&lts[i], LTHREAD_PRIORITY_MAX - i)
		);
		lthread_launch(&lts[i]);
	}

	/* deleting occurs onle after finishing */
	for(int i = 0; i < LTHREAD_QUANTITY; i++) {
		lthread_delete(&lts[i]);
	}

	test_assert_equal(done, LTHREAD_QUANTITY);
}

static void *run3(void *arg) {
	test_emit('b');
	done++;
	return NULL;
}

TEST_CASE("Test executing order") {
	struct lthread lt1, lt2;

	done = 0;

	lthread_init(&lt1, run1, NULL);
	lthread_priority_set(&lt1, LTHREAD_PRIORITY_MAX - 1);

	lthread_init(&lt2, run3, NULL);
	lthread_priority_set(&lt2, LTHREAD_PRIORITY_MAX);

	/* prevent scheduling to avoid executing one
	 * before adding another to runq
	 */
	sched_lock();
	{
		lthread_launch(&lt1);
		lthread_launch(&lt2);
	}
	sched_unlock();

	lthread_delete(&lt1);
	lthread_delete(&lt2);

	test_assert_emitted("ba");
}

TEST_CASE("Test several launches") {
	struct lthread lt;

	done = 0;

	lthread_init(&lt, run1, NULL);
	lthread_priority_set(&lt, LTHREAD_PRIORITY_MAX);

	lthread_launch(&lt);
	ksleep(0);
	lthread_launch(&lt);
	ksleep(0);

	test_assert_equal(2, done);

	lthread_delete(&lt);
}

