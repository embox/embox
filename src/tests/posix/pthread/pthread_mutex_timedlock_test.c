#include <embox/test.h>

#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<stdio.h>
#include <sched.h>

EMBOX_TEST_SUITE("posix/pthread_mutex_timedlock");

#define TIMED_OUT 1

struct thread_arg {
	volatile int go;
	int sleep_time;
	pthread_mutex_t mutex;
};

static void set_normalized_timespec(struct timespec *ts,
		time_t sec, long nsec) {
	while (nsec >= NSEC_PER_SEC) {
		nsec -= NSEC_PER_SEC;
		sec++;
	}

	while (nsec < 0) {
		nsec += NSEC_PER_SEC;
		sec--;
	}

	ts->tv_sec = sec;
	ts->tv_nsec = nsec;
}

static struct timespec timespec_add(struct timespec t1,
		struct timespec t2) {
	struct timespec ts;

	set_normalized_timespec(&ts, t1.tv_sec + t2.tv_sec,
			t1.tv_nsec + t2.tv_nsec);

	return ts;
}

static void * func1(void *arg) {
	struct thread_arg *targ = (struct thread_arg *)arg;

	pthread_mutex_lock(&targ->mutex);
	targ->go = 1;
	sleep(targ->sleep_time);
	pthread_mutex_unlock(&targ->mutex);
	return NULL;
}

static int test_case(int sleep_time1, int sleep_time2)
{
	int ret;
	int status = 0;
	pthread_t thread1;
	struct timespec wait;
	struct timespec relative_time;
	struct thread_arg targ = { .go = 0, .sleep_time = sleep_time1 };

	ret = pthread_create(&thread1, NULL, &func1, (void *)&targ);
	if (ret != 0) {
		test_fail("Failed pthread_create\n");
	}

	while (!targ.go) { sched_yield(); }

	wait.tv_sec = sleep_time2;
	wait.tv_nsec = 0;
	clock_gettime(CLOCK_REALTIME, &relative_time);
	wait = timespec_add(wait, relative_time);

	ret = pthread_mutex_timedlock(&targ.mutex, &wait);
	if (ret != 0) {
		status = TIMED_OUT;
	} else {
		pthread_mutex_unlock(&targ.mutex);
	}

	pthread_join(thread1, NULL);
	return status;
}

TEST_CASE("Thread times out on mutex") {
	test_assert_equal(test_case(2, 1), TIMED_OUT);
}

TEST_CASE("Thread does not time out on mutex") {
	test_assert_not_equal(test_case(1, 2), TIMED_OUT);
}
