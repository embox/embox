/**
 * @file
 *
 * @date Aug 23, 2013
 * @author: Anton Bondarev
 */

#include <pthread.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("posix/pthread_mutex api");


TEST_CASE("Initialize mutex with PTHREAD_MUTEX_INITIALIZER macro") {
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	test_assert_zero(pthread_mutex_lock(&m));
	test_assert_equal(-EBUSY, pthread_mutex_trylock(&m));
	test_assert_zero(pthread_mutex_unlock(&m));
}

TEST_CASE("Initialize mutex with PTHREAD_RMUTEX_INITIALIZER macro") {
	pthread_mutex_t m = PTHREAD_RMUTEX_INITIALIZER;
	test_assert_zero(pthread_mutex_lock(&m));
	test_assert_zero(pthread_mutex_trylock(&m));
	test_assert_zero(pthread_mutex_unlock(&m));
	test_assert_zero(pthread_mutex_unlock(&m));
}

TEST_CASE("Mutex with type PTHREAD_MUTEX_ERRORCHECK correctness") {
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&mutex, &mutexattr);

	test_assert_equal(-EPERM, pthread_mutex_unlock(&mutex));
	test_assert_zero(pthread_mutex_lock(&mutex));
	test_assert_equal(-EDEADLK, pthread_mutex_lock(&mutex));
	test_assert_zero(pthread_mutex_unlock(&mutex));
	test_assert_equal(-EPERM, pthread_mutex_unlock(&mutex));
}

TEST_CASE("Mutex with type PTHREAD_MUTEX_RECURSIVE correctness") {
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &mutexattr);

	test_assert_equal(-EPERM, pthread_mutex_unlock(&mutex));
	test_assert_zero(pthread_mutex_lock(&mutex));
	test_assert_zero(pthread_mutex_lock(&mutex));
	test_assert_zero(pthread_mutex_unlock(&mutex));
	test_assert_zero(pthread_mutex_unlock(&mutex));
	test_assert_equal(-EPERM, pthread_mutex_unlock(&mutex));
}

TEST_CASE("Mutex with type PTHREAD_MUTEX_DEFAULT correctness") {
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_unlock(&mutex);
	test_assert_zero(pthread_mutex_lock(&mutex));
	test_assert_equal(-EBUSY, pthread_mutex_trylock(&mutex));
	test_assert_zero(pthread_mutex_unlock(&mutex));
}
