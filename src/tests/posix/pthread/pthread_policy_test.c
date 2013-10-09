/**
 * @file
 *
 * @date Aug 23, 2013
 * @author: Anton Bondarev
 */

#include <pthread.h>
#include <embox/test.h>
#include <kernel/thread.h>
#include <err.h>
#include <embox/test.h>
#include <kernel/sched.h>

static pthread_t low, medium, high, low_other;

EMBOX_TEST_SUITE("posix/scheduling policy");

TEST_SETUP(setup);

TEST_CASE("SCHED_FIFO corectness") {
	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(low_other, NULL));
	test_assert_zero(thread_join(medium, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcdefd");
}

static void *low_run(void *arg) {
	test_emit('a');
	thread_launch(medium);
	thread_launch(low_other);
	test_emit('f');
	return NULL;
}

static void *low_other_run(void *arg) {
	for (int i = 0; i < 1000000; i++);
	test_emit('d');
	return NULL;
}

static void *medium_run(void *arg) {
	test_emit('b');
	thread_launch(high);
	for (int i = 0; i < 1000000; i++);
	test_emit('e');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('c');
	for (int i = 0; i < 1000000; i++);
	test_emit('d');
	return NULL;
}

static int setup(void) {
	sched_priority_t low_p = 200, medium_p = 245, high_p = 250;
	struct sched_param low_param, medium_param, high_param;

	low_param.sched_priority = low_p;
	medium_param.sched_priority = medium_p;
	high_param.sched_priority = high_p;

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(err(low));

	low_other = thread_create(THREAD_FLAG_SUSPENDED, low_other_run, NULL);
	test_assert_zero(err(low_other));

	medium = thread_create(THREAD_FLAG_SUSPENDED, medium_run, NULL);
	test_assert_zero(err(medium));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, NULL);
	test_assert_zero(err(high));

	pthread_setschedparam(low, SCHED_OTHER, &low_param);
	pthread_setschedparam(low_other, SCHED_OTHER, &low_param);
	pthread_setschedparam(medium, SCHED_FIFO, &medium_param);
	pthread_setschedparam(high, SCHED_FIFO, &high_param);

	return 0;
}
