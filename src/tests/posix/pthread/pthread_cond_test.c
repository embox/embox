/**
 * @file
 *
 * @date Dec 16, 2013
 * @author: Anton Bondarev
 */
#include <pthread.h>
#include <time.h>
#include <embox/test.h>
#include <kernel/time/time.h>

EMBOX_TEST_SUITE("posix/pthread_cond_api");

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

TEST_CASE("Initialize pthread_cond with PTHREAD_COND_INITIALIZER macro") {
	int ret;
	struct timespec timeout;
	struct timespec relative_time;

	timeout.tv_sec = 0;
	timeout.tv_nsec = 10000000;

	clock_gettime(CLOCK_REALTIME, &relative_time);
	timeout = timespec_add(timeout, relative_time);

	pthread_mutex_lock(&mut);
	ret = pthread_cond_timedwait(&cond, &mut, &timeout);
	pthread_mutex_unlock(&mut);

	test_assert(ret == ETIMEDOUT);
}
