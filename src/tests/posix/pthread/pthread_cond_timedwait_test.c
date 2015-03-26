#include <embox/test.h>
#include <kernel/thread/sync/cond.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <util/err.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

static pthread_cond_t *cond;
static pthread_mutex_t *mut;
struct timespec *time_to_sleep;
struct timespec help;


EMBOX_TEST_SUITE("Timed condition variable test");

TEST_SETUP(setup);

TEST_TEARDOWN(teardown);


// with time interval
TEST_CASE("Time-out occurring")
{
	int res;
	time_to_sleep -> tv_sec = 5;
	time_to_sleep -> tv_nsec = 0;


	pthread_mutex_lock(mut);
	res = pthread_cond_timedwait(cond, mut, time_to_sleep);
	pthread_mutex_unlock(mut);

	test_assert(res != 0); // Macros ETIMEDOUT should be defined in phtread.h for this error
}

/*
TEST_CASE("Time-out occurring")
{
	int res;
	struct timeval *BegTime;
	struct timeval help_1;

	BegTime = malloc (sizeof(help_1));

	// Using absolute time
	// What function should we use here?
	gettimeofday(BegTime, NULL);
	time_to_sleep -> tv_sec = BegTime -> tv_sec + 5;
	time_to_sleep -> tv_nsec = BegTime -> tv_usec * 1000UL;

	pthread_mutex_lock(mut);
	res = pthread_cond_timedwait(cond, mut, time_to_sleep);
	pthread_mutex_unlock(mut);

	test_assert(res != 0);
}
*/

static int setup(void)
{
	time_to_sleep = malloc (sizeof(help));
	cond = malloc (sizeof(pthread_cond_t));
	mut = malloc (sizeof(pthread_mutex_t));

	pthread_mutex_init(mut, NULL);
	pthread_cond_init(cond, NULL);

	return 0;
}

static int teardown(void)
{
	free(mut);
	free(cond);

	return 0;
}

