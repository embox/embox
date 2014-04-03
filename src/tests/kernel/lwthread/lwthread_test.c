/**
 * @file lwthread_test.c
 *
 * @brief simple test for lwthreads
 *
 * @author Andrey Kokorev
 */
#include <err.h>
#include <embox/test.h>
#include <kernel/sched.h>
#include <kernel/lwthread/lwthread.h>
#include <kernel/lwthread/lwthread_priority.h>
#include <kernel/time/ktime.h>

#define LWTHREADS_QUANTITY OPTION_GET(NUMBER, lwthreads_quantity)

EMBOX_TEST_SUITE("test for lwthread API");

static volatile int done = 0;

static void *run1(void *arg) {
	test_emit('a');
	done++;
	return NULL;
}

TEST_CASE("Launch simple lwthread") {
	struct lwthread *lwt;

	lwt = lwthread_create(run1, NULL);
	test_assert_zero(err(lwt));
	lwthread_launch(lwt);

	/* Spin, waiting lwthread finished */
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

TEST_CASE("Call sched from lwthread") {
	struct lwthread *lwt;

	done = 0;

	lwt = lwthread_create(run_resched, NULL);
	test_assert_zero(err(lwt));
	lwthread_launch(lwt);

	/* Spin, waiting lwthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}
}

static void *run2(void *arg) {
	done++;
	return NULL;
}

TEST_CASE("Create lwthreads with different priorities") {
	done = 0;

	for(int i = 0; i < LWTHREADS_QUANTITY; i++) {
		struct lwthread *lwt;

		lwt = lwthread_create(run2, NULL);
		test_assert_zero(err(lwt));
		test_assert_zero(
			lwthread_priority_set(lwt, LWTHREAD_PRIORITY_MIN + i)
		);
		lwthread_launch(lwt);
	}

	/* Spin, waiting all lwthreads finished */
	while(1) {
		if(done == LWTHREADS_QUANTITY) break;
		ksleep(0);
	}
}

static void *run3(void *arg) {
	test_emit('b');
	done++;
	return NULL;
}

TEST_CASE("Test executing order") {
	struct lwthread *lwt1, *lwt2;

	done = 0;

	lwt1 = lwthread_create(run1, NULL);
	test_assert_zero(err(lwt1));
	lwthread_priority_set(lwt1, LWTHREAD_PRIORITY_MIN);

	lwt2 = lwthread_create(run3, NULL);
	test_assert_zero(err(lwt2));
	lwthread_priority_set(lwt2, LWTHREAD_PRIORITY_MAX);

	/* prevent scheduling to avoid executing one
	 * before adding another to runq
	 */
	sched_lock();
	{
		lwthread_launch(lwt1);
		lwthread_launch(lwt2);
	}
	sched_unlock();

	while(done < 2) {
		ksleep(0);
	}
	test_assert_emitted("ba");
}
