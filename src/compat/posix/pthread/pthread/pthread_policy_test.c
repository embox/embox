/**
 * @file
 *
 * @date 09.10.13
 * @author: Vita Loginova
 */

#include <pthread.h>
#include <embox/test.h>

static pthread_t low, medium, high, low_other;
pthread_attr_t *low_attr_p, *medium_attr_p, *high_attr_p;

EMBOX_TEST_SUITE("posix/scheduling policy");

static void *low_run(void *arg);
static void *low_other_run(void *arg);
static void *medium_run(void *arg);
static void *high_run(void *arg);

TEST_CASE("SCHED_FIFO corectness") {
	int low_p = 200, medium_p = 245, high_p = 250;
	struct sched_param low_param, medium_param, high_param;
	pthread_attr_t low_attr, medium_attr, high_attr;

	low_attr_p = &low_attr;
	medium_attr_p = &medium_attr;
	high_attr_p = &high_attr;

	low_param.sched_priority = low_p;
	medium_param.sched_priority = medium_p;
	high_param.sched_priority = high_p;

	test_assert_zero(pthread_attr_init(&low_attr));
	test_assert_zero(pthread_attr_init(&medium_attr));
	test_assert_zero(pthread_attr_init(&high_attr));

	pthread_attr_setschedparam(&low_attr, &low_param);
	pthread_attr_setschedparam(&medium_attr, &medium_param);
	pthread_attr_setschedparam(&high_attr, &high_param);

	pthread_attr_setschedpolicy(&low_attr, SCHED_OTHER);
	pthread_attr_setschedpolicy(&medium_attr, SCHED_FIFO);
	pthread_attr_setschedpolicy(&high_attr, SCHED_FIFO);

	test_assert_zero(pthread_create(&low, &low_attr, low_run, NULL));

	test_assert_zero(pthread_join(low, NULL));
	test_assert_zero(pthread_join(low_other, NULL));
	test_assert_zero(pthread_join(medium, NULL));
	test_assert_zero(pthread_join(high, NULL));
	test_assert_emitted("abcdefg");

	test_assert_zero(pthread_attr_destroy(&low_attr));
	test_assert_zero(pthread_attr_destroy(&medium_attr));
	test_assert_zero(pthread_attr_destroy(&high_attr));
}

static void *low_run(void *arg) {
	test_emit('a');
	test_assert_zero(pthread_create(&medium, medium_attr_p, medium_run, NULL));
	test_assert_zero(pthread_create(&low_other, low_attr_p, low_other_run, NULL));
	for (int i = 0; i < 20000000; i++);
	test_emit('g');
	return NULL;
}

static void *low_other_run(void *arg) {
	test_emit('f');
	return NULL;
}

static void *medium_run(void *arg) {
	test_emit('b');
	test_assert_zero(pthread_create(&high, high_attr_p, high_run, NULL));
	for (int i = 0; i < 20000000; i++);
	test_emit('e');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('c');
	for (int i = 0; i < 20000000; i++);
	test_emit('d');
	return NULL;
}
