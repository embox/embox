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
#include <kernel/lwthread.h>
#include <kernel/time/ktime.h>

EMBOX_TEST_SUITE("test for lwthread API");

static volatile int done = 0;

static void *run(void *arg) {
	test_emit('a');
	done = 1;
	return NULL;
}


TEST_CASE("Launch simple lwthread") {
	struct lwthread *lwt;

	lwt = lwthread_create(run, NULL);
	test_assert_zero(err(lwt));
	lwthread_launch(lwt);

	/* Spin, waiting lwthread finished */
	while(1) {
		if(done == 1) break;
		ksleep(0);
	}

	lwthread_free(lwt);
	test_assert_emitted("a");
}


