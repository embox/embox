/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    12.08.2014
 */

#include <err.h>
#include <embox/test.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/schedee/current.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>
#include <kernel/lthread/lthread_sched_wait.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <kernel/schedee/sync/mutex.h>
#include <kernel/lthread/sync/mutex.h>
#include <kernel/thread/sync/mutex.h>

EMBOX_TEST_SUITE("sched_wait_*_lthread test");

static int done = 0, ready = 0;

static void *sched_wait_timeout_run(void *arg) {
	int res, timeout;
	timeout = (int)arg;
	sched_wait_prepare_lthread(timeout);
	if ((res = sched_wait_timeout_lthread()) == -EAGAIN) {
		return NULL;
	}
	sched_wait_cleanup_lthread();

	done = 1;

	return (void *)res;
}

TEST_CASE("sched_wait_timeout: timeout is exceeded") {
	struct lthread *lt;
	int timeout = 20;

	done = 0;

	lt = lthread_create(sched_wait_timeout_run, (void *)timeout);
	test_assert_zero(err(lt));
	lthread_launch(lt);

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(timeout);
	}

	test_assert_equal((int)lt->run_ret, -ETIMEDOUT);

	lthread_delete(lt);
}

TEST_CASE("sched_wait_timeout: wakeup before timeout is exceeded") {
	struct lthread *lt;
	int timeout = 20;

	done = 0;

	lt = lthread_create(sched_wait_timeout_run, (void *)timeout);
	test_assert_zero(err(lt));
	lthread_launch(lt);

	sched_wakeup(&lt->schedee);

	ksleep(0);

	test_assert_equal(done, 1);
	test_assert_equal((int)lt->run_ret, 0);

	lthread_delete(lt);
}

TEST_CASE("sched_wait_timeout: SCHED_TIMEOUT_INFINITE") {
	struct lthread *lt;
	int timeout = SCHED_TIMEOUT_INFINITE;

	done = 0;

	lt = lthread_create(sched_wait_timeout_run, (void *)timeout);
	test_assert_zero(err(lt));
	lthread_launch(lt);

	sched_wakeup(&lt->schedee);

	ksleep(0);

	test_assert_equal(done, 1);
	test_assert_equal((int)lt->run_ret, 0);

	lthread_delete(lt);
}

static void *sched_wait_timeout_macro_run(void *arg) {
	int res, timeout = (int)arg;
	if ((res = SCHED_WAIT_TIMEOUT_LTHREAD(ready, timeout)) == -EAGAIN) {
		return NULL;
	}

	done = 1;

	return (void *)res;
}

TEST_CASE("SCHED_WAIT_TIMEOUT_LTHREAD: wakeup before timeout is exceeded") {
	struct lthread *lt;
	int timeout = 20;

	done = 0;
	ready = 0;

	lt = lthread_create(sched_wait_timeout_macro_run, (void *)timeout);
	test_assert_zero(err(lt));
	lthread_launch(lt);

	ksleep(0);

	ready = 1;

	sched_wakeup(&lt->schedee);

	ksleep(0);

	test_assert_equal(done, 1);
	test_assert_equal((int)lt->run_ret, 0);

	lthread_delete(lt);
}

TEST_CASE("SCHED_WAIT_TIMEOUT_LTHREAD: timeout exceeded") {
	struct lthread *lt;
	int timeout = 20;

	done = 0;
	ready = 0;

	lt = lthread_create(sched_wait_timeout_macro_run, (void *)timeout);
	test_assert_zero(err(lt));
	lthread_launch(lt);

	while(1) {
		if(done == 1) break;
		ksleep(timeout);
	}

	test_assert_equal(done, 1);
	test_assert_equal((int)lt->run_ret, -ETIMEDOUT);

	lthread_delete(lt);
}

TEST_CASE("SCHED_WAIT_TIMEOUT_LTHREAD: SCHED_TIMEOUT_INFINITE") {
	struct lthread *lt;
	int timeout = SCHED_TIMEOUT_INFINITE;

	done = 0;
	ready = 0;

	lt = lthread_create(sched_wait_timeout_macro_run, (void *)timeout);
	test_assert_zero(err(lt));
	lthread_launch(lt);

	ksleep(20);

	ready = 1;
	sched_wakeup(&lt->schedee);

	ksleep(0);

	test_assert_equal(done, 1);
	test_assert_equal((int)lt->run_ret, 0);

	lthread_delete(lt);
}
