/**
 * @file
 * @brief   Test pthread_barrier functions
 * @date    12.03.2020
 * @author  Sobhan Mondal <inro20fdm@gmail.com>
 */

#include <pthread.h>
#include <embox/test.h>
#include <unistd.h>
#include <stdlib.h>

EMBOX_TEST_SUITE("posix/pthread_barrier_api");

TEST_CASE ("Initialize barrier with pthread_barrier_init()") {
	pthread_barrier_t barrier = NULL; 
	pthread_barrierattr_t attr;
	test_assert_zero(pthread_barrierattr_init(&attr));
	test_assert_zero(pthread_barrier_init(&barrier, &attr, 1));
	test_assert_equal(pthread_barrier_wait(&barrier), 
	    PTHREAD_BARRIER_SERIAL_THREAD);
	test_assert_zero(pthread_barrier_destroy(&barrier));
	test_assert_zero(pthread_barrierattr_destroy(&attr));
}

static void *thread(void *arg) {
	sleep(1);
	test_assert_equal(pthread_barrier_wait(arg), 
	    PTHREAD_BARRIER_SERIAL_THREAD);
	return NULL;
}

TEST_CASE("Testing pthread_barrier_wait()") {
	pthread_t tid;
	void *vp;
	pthread_barrier_t barrier = NULL;

	test_assert_zero(pthread_barrier_init(&barrier, NULL, 2));
	test_assert_zero(pthread_create(&tid, NULL, thread, &barrier));
	test_assert_zero(pthread_barrier_wait(&barrier));
	test_assert_zero(pthread_join(tid, &vp));
	test_assert_zero(pthread_barrier_destroy(&barrier));
}

static void *thread1(void *arg) {
	sleep(1);
	test_assert_equal(pthread_barrier_wait(arg), 
	    PTHREAD_BARRIER_SERIAL_THREAD);
	return NULL;
}

static void *thread2(void *arg) {
	sleep(2);
	test_assert_zero(pthread_barrier_wait(arg));
	return NULL;
}

static void *thread3(void *arg) {
	sleep(3);
	test_assert_equal(pthread_barrier_wait(arg), 
	    PTHREAD_BARRIER_SERIAL_THREAD);
	return NULL;
}

TEST_CASE("Testing pthread_barrier_wait() with multiple round") {
	pthread_t tid1, tid2, tid3;
	void *vp;
	pthread_barrier_t barrier = NULL;

	test_assert_zero(pthread_barrier_init(&barrier, NULL, 2));
	test_assert_zero(pthread_create(&tid1, NULL, thread1, &barrier));
	test_assert_zero(pthread_create(&tid2, NULL, thread2, &barrier));
	test_assert_zero(pthread_create(&tid3, NULL, thread3, &barrier));
	test_assert_zero(pthread_barrier_wait(&barrier));
	test_assert_zero(pthread_join(tid1, &vp));
	test_assert_zero(pthread_join(tid2, &vp));
	test_assert_zero(pthread_join(tid3, &vp));
	test_assert_zero(pthread_barrier_destroy(&barrier));
}
