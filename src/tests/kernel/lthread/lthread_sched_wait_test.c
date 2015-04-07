/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    12.08.2014
 */

#include <util/err.h>
#include <embox/test.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/sched/current.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_sched_wait.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <kernel/sched/sync/mutex.h>
#include <kernel/lthread/sync/mutex.h>
#include <kernel/thread/sync/mutex.h>

EMBOX_TEST_SUITE("sched_wait_*_lthread test");

struct lt_test {
	struct lthread lt;
	int timeout;
	int res;
};

static int done = 0, ready = 0;

static int sched_wait_timeout_run(struct lthread *self) {
	int res;
	struct lt_test *lt_test = (struct lt_test *)self;

	sched_wait_prepare_lthread(self, lt_test->timeout);

	if ((res = sched_wait_timeout_lthread(self)) == -EAGAIN) {
		return 0;
	}
	sched_wait_cleanup_lthread(self);

	lt_test->res = res;

	done = 1;

	return 0;
}

TEST_CASE("sched_wait_timeout: timeout is exceeded") {
	struct lt_test lt_test;
	lt_test.timeout = 20;

	done = 0;

	lthread_init(&(lt_test.lt), sched_wait_timeout_run);
	lthread_launch(&(lt_test.lt));

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(lt_test.timeout);
	}

	test_assert_equal(done, 1);
	test_assert_equal(lt_test.res, -ETIMEDOUT);

	lthread_reset(&(lt_test.lt));
}

TEST_CASE("sched_wait_timeout: wakeup before timeout is exceeded") {
	struct lt_test lt_test;
	lt_test.timeout = 200;

	done = 0;

	lthread_init(&lt_test.lt, sched_wait_timeout_run);
	lthread_launch(&lt_test.lt);

	lthread_launch(&lt_test.lt);

	test_assert_equal(done, 1);
	test_assert_equal(lt_test.res, 0);

	lthread_reset(&lt_test.lt);
}

TEST_CASE("sched_wait_timeout: SCHED_TIMEOUT_INFINITE") {
	struct lt_test lt_test;
	lt_test.timeout = SCHED_TIMEOUT_INFINITE;

	done = 0;

	lthread_init(&lt_test.lt, sched_wait_timeout_run);
	lthread_launch(&lt_test.lt);

	lthread_launch(&lt_test.lt);

	ksleep(0);

	test_assert_equal(done, 1);
	test_assert_equal(lt_test.res, 0);

	lthread_reset(&lt_test.lt);
}

static int sched_wait_timeout_macro_run(struct lthread *self) {
	int res;
	struct lt_test *lt_test = (struct lt_test *)self;

	res = SCHED_WAIT_TIMEOUT_LTHREAD(self, ready, lt_test->timeout);
	if (res == -EAGAIN) {
		return 0;
	}

	done = 1;

	lt_test->res = res;

	return 0;
}

TEST_CASE("SCHED_WAIT_TIMEOUT_LTHREAD: wakeup before timeout is exceeded") {
	struct lt_test lt_test;
	lt_test.timeout = 20;

	done = 0;
	ready = 0;

	lthread_init(&lt_test.lt, sched_wait_timeout_macro_run);
	lthread_launch(&lt_test.lt);

	ksleep(0);

	ready = 1;

	lthread_launch(&lt_test.lt);

	ksleep(0);

	test_assert_equal(done, 1);
	test_assert_equal(lt_test.res, 0);

	lthread_reset(&lt_test.lt);
}

TEST_CASE("SCHED_WAIT_TIMEOUT_LTHREAD: timeout exceeded") {
	struct lt_test lt_test;
	lt_test.timeout = 20;

	done = 0;
	ready = 0;

	lthread_init(&lt_test.lt, sched_wait_timeout_macro_run);
	lthread_launch(&lt_test.lt);

	while(1) {
		if(done == 1) break;
		ksleep(lt_test.timeout);
	}

	test_assert_equal(done, 1);
	test_assert_equal(lt_test.res, -ETIMEDOUT);

	lthread_reset(&lt_test.lt);
}

TEST_CASE("SCHED_WAIT_TIMEOUT_LTHREAD: SCHED_TIMEOUT_INFINITE") {
	struct lt_test lt_test;
	lt_test.timeout = SCHED_TIMEOUT_INFINITE;

	done = 0;
	ready = 0;

	lthread_init(&lt_test.lt, sched_wait_timeout_macro_run);
	lthread_launch(&lt_test.lt);

	ksleep(20);

	ready = 1;
	lthread_launch(&lt_test.lt);

	ksleep(0);

	test_assert_equal(done, 1);
	test_assert_equal(lt_test.res, 0);

	lthread_reset(&lt_test.lt);
}
