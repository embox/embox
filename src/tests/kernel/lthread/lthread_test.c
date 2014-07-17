/**
 * @file lthread_test.c
 *
 * @brief simple test for lthreads
 *
 * @author Andrey Kokorev
 */
#include <err.h>
#include <embox/test.h>
#include <kernel/sched.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>
#include <kernel/time/ktime.h>

#define lthreadS_QUANTITY OPTION_GET(NUMBER, lthreads_quantity)

EMBOX_TEST_SUITE("test for lthread API");

static volatile int done = 0;

static void *run1(void *arg) {
	test_emit('a');
	done++;
	return NULL;
}

TEST_CASE("Launch simple lthread") {
	struct lthread *lwt;

	lwt = lthread_create(run1, NULL);
	test_assert_zero(err(lwt));
	lthread_launch(lwt);

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}

	test_assert_emitted("a");
}

static void *run_resched(void *arg) {
	ksleep(0);
	done = 1;
	return NULL;
}

TEST_CASE("Call sched from lthread") {
	struct lthread *lwt;

	done = 0;

	lwt = lthread_create(run_resched, NULL);
	test_assert_zero(err(lwt));
	lthread_launch(lwt);

	/* Spin, waiting lthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}
}

static void *run2(void *arg) {
	done++;
	return NULL;
}

TEST_CASE("Create lthreads with different priorities") {
	done = 0;

	for(int i = 0; i < lthreadS_QUANTITY; i++) {
		struct lthread *lwt;

		lwt = lthread_create(run2, NULL);
		test_assert_zero(err(lwt));
		test_assert_zero(
			runnable_priority_set(&lwt->runnable, LWTHREAD_PRIORITY_MIN + i)
		);
		lthread_launch(lwt);
	}

	/* Spin, waiting all lthreads finished */
	while(1) {
		if(done == lthreadS_QUANTITY) break;
		ksleep(0);
	}
}

static void *run3(void *arg) {
	test_emit('b');
	done++;
	return NULL;
}

TEST_CASE("Test executing order") {
	struct lthread *lwt1, *lwt2;

	done = 0;

	lwt1 = lthread_create(run1, NULL);
	test_assert_zero(err(lwt1));
	runnable_priority_set(&lwt1->runnable, LWTHREAD_PRIORITY_MIN);

	lwt2 = lthread_create(run3, NULL);
	test_assert_zero(err(lwt2));
	runnable_priority_set(&lwt2->runnable, LWTHREAD_PRIORITY_MAX);

	/* prevent scheduling to avoid executing one
	 * before adding another to runq
	 */
	sched_lock();
	{
		lthread_launch(lwt1);
		lthread_launch(lwt2);
	}
	sched_unlock();

	while(done < 2) {
		ksleep(0);
	}
	test_assert_emitted("ba");
}
