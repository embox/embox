/**
 * @file
 * @brief Tests for sleep function
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <unistd.h>
#include <stdlib.h>
#include <kernel/time/time.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread/thread_sched_wait.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>

EMBOX_TEST_SUITE("sleep suite");

#define EPSILON_BORDER 30
#define TIME_TO_SLEEP  (100 * USEC_PER_MSEC)
#define NUM_THREADS     8
#define BENCH_LOOPS   300

/**
 *  sleep( any_time )
 *  assert that real time for sleep different with any_time is less than EPSILON_BORDER
 */

TEST_CASE("one sleep") {
	clock_t cur_time, epsilon;

	cur_time = clock();
	usleep(TIME_TO_SLEEP);
	epsilon = clock();

	test_assert_true(epsilon >= cur_time);
	epsilon = epsilon - cur_time;

	test_assert_true(TIME_TO_SLEEP / USEC_PER_MSEC < epsilon);

	test_assert_true(epsilon < (TIME_TO_SLEEP / USEC_PER_MSEC + EPSILON_BORDER));
}

static void * handler_timeout(void* args) {
	struct waitq *wq = (struct waitq*) args;
	test_assert_equal(-ETIMEDOUT, SCHED_WAIT_TIMEOUT(0, 10));
	waitq_wakeup(wq, 1);
	return NULL;
}

TEST_CASE("timeout sleep") {
	pthread_t t;
	struct waitq wq;
	struct waitq_link wql;
	clock_t remain;

	waitq_init(&wq);
	waitq_link_init(&wql);
	waitq_wait_prepare(&wq, &wql);

	test_assert_zero(pthread_create(&t, 0, handler_timeout, (void*) &wq));
	test_assert_not_null(t);

	test_assert_zero(sched_wait_timeout(1000, &remain));
	test_assert_not_zero(remain);

	waitq_wait_cleanup(&wq, &wql);
}

/**
 *  run ~3 threads (~3 times) with 1,2,3 sec wait
 *  assert right done sequence of threads
 */

static void * handler1(void* args) {
	usleep(TIME_TO_SLEEP * (uint32_t)(uintptr_t)args);
	test_emit('0' + (uint32_t)(uintptr_t)args);
	return NULL;
}

TEST_CASE("simple multi-threaded check") {
	pthread_t t1, t2, t3;

	/* Start threads */

	test_assert_zero(pthread_create(&t1, 0, handler1, (void *) 1));
	test_assert_not_null(t1);
	test_assert_zero(pthread_create(&t2, 0, handler1, (void *) 2));
	test_assert_not_null(t2);
	test_assert_zero(pthread_create(&t3, 0, handler1, (void *) 3));
	test_assert_not_null(t3);

	/* join thread */
	test_assert_zero(pthread_join(t1, NULL));
	test_assert_zero(pthread_join(t2, NULL));
	test_assert_zero(pthread_join(t3, NULL));

	test_assert_emitted("123");
}

/**
 * run NUM_THREADS threads and with progressive time to sleep
 * after execute buffer2 must be "87654321"
 */
static void * handler2(void* args) {
	usleep(TIME_TO_SLEEP * (NUM_THREADS - (uint32_t)(uintptr_t)args) + 1);
	test_emit('1' + (uint32_t)(uintptr_t)args);
	return NULL;
}

TEST_CASE("sleep sort") {
	uint32_t i;
	pthread_t t[NUM_THREADS];

	for (i = 0; i < NUM_THREADS; i++) {
		test_assert_zero(
				pthread_create(&t[i], 0, handler2, (void *)(uintptr_t)i));
		test_assert_not_null(t[i]);
	}
	for (i = 0; i < NUM_THREADS; i++) {
		test_assert_zero(pthread_join(t[i], NULL));
	}
	test_assert_emitted("87654321");
}

TEST_CASE("sleep 0 seconds") {
	test_emit('1');
	usleep(0);
	test_emit('2');
	test_assert_emitted("12");
}

TEST_CASE("many sleeps") {

	for (int i = 0; i < BENCH_LOOPS; i++) {
		usleep((i % 17) * USEC_PER_MSEC);
	}
}


