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

EMBOX_TEST_SUITE("test for lwthread API");


static void *run(void *arg) {
	test_emit('a');
	return NULL;
}


TEST_CASE("Launch simple lwthread") {
	struct lwthread *lwt;
	lwt = lwthread_create(run, NULL);
	test_assert_zero(err(lwt));
	lwthread_launch(lwt);

	lwthread_free(lwt);
	/* Not sure if it would be completed here */
	test_assert_emitted("a");
}
