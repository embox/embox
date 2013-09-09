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
	test_assert_zero(pthread_mutex_unlock(&m));
}
