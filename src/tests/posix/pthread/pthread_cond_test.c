/**
 * @file
 *
 * @date Dec 16, 2013
 * @author: Anton Bondarev
 */
#include <pthread.h>
#include <time.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("posix/pthread_cond_api");

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

TEST_CASE("Initialize pthread_cond with PTHREAD_COND_INITIALIZER macro") {
	int ret;
	struct timespec timeout;

	timeout.tv_sec = 0;
	timeout.tv_nsec = 10000000;

	pthread_mutex_lock(&mut);
	ret = pthread_cond_timedwait(&cond, &mut, &timeout);
	pthread_mutex_unlock(&mut);

	test_assert (ret == ETIMEDOUT);
}
