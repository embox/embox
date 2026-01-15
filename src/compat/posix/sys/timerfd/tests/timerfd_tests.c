/**
 * @file
 * @brief Tests for timerfd.
 * @details Sets the timer and calls read() on it once.
 *
 * @date 04.04.17
 * @author Kirill Smirenko
 */

#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/timerfd.h>

#include <embox/test.h>

#include <kernel/printk.h>
#include <kernel/time/time.h>

EMBOX_TEST_SUITE("timerfd suite");

// a difference of less than 32 milliseconds is assumed okay
#define MAX_ALLOWED_TIME_DIFF 32LL * NSEC_PER_MSEC

static int timespec_is_zero(const struct timespec *ts) {
	return (ts->tv_sec == 0) && (ts->tv_nsec == 0);
}

static int timespec_equals(const struct timespec *ts1,
		const struct timespec *ts2) {
	return (ts1->tv_sec == ts2->tv_sec) && (ts1->tv_nsec == ts2->tv_nsec);
}

static int timespec_approx_equals(const struct timespec *ts1,
		const struct timespec *ts2) {
	time64_t diff;
	diff = timespec_to_ns(ts1) - timespec_to_ns(ts2);
	return (diff > -MAX_ALLOWED_TIME_DIFF) && (diff < MAX_ALLOWED_TIME_DIFF);
}

TEST_CASE("timerfd get (disarmed)") {
	int timer_fd = -1;
	struct itimerspec curr_value;

	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	test_assert_true(timer_fd >= 0);

	timerfd_gettime(timer_fd, &curr_value);
	test_assert_true(timespec_is_zero(&curr_value.it_value));
	test_assert_true(timespec_is_zero(&curr_value.it_interval));

	close(timer_fd);
}

TEST_CASE("timerfd set and get (one-time timer)") {
	int timer_fd = -1;
	struct itimerspec timer_setting, curr_value;
	struct timespec ts_now, expected;

	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	test_assert_true(timer_fd >= 0);

	timer_setting.it_value.tv_sec = 1;
	timer_setting.it_value.tv_nsec = 0;
	timer_setting.it_interval.tv_sec = 0;
	timer_setting.it_interval.tv_nsec = 0;
	clock_gettime(CLOCK_MONOTONIC, &ts_now);
	expected = timespec_add(ts_now, timer_setting.it_value);

	timerfd_settime(timer_fd, 0, &timer_setting, NULL);

	// first gettime: timer not expired yet
	usleep(USEC_PER_SEC / 4);
	timerfd_gettime(timer_fd, &curr_value);

	clock_gettime(CLOCK_MONOTONIC, &ts_now);
	expected = timespec_sub(expected, ts_now);
	test_assert_true(timespec_approx_equals(&curr_value.it_value, &expected));
	test_assert_true(timespec_is_zero(&curr_value.it_interval));

	// second gettime: timer expired
	usleep(USEC_PER_SEC * 2);
	timerfd_gettime(timer_fd, &curr_value);
	test_assert_true(timespec_is_zero(&curr_value.it_value));
	test_assert_true(timespec_is_zero(&curr_value.it_interval));

	close(timer_fd);
}

TEST_CASE("timerfd set and get (one-time, absolute time set)") {
	int timer_fd = -1;
	struct itimerspec timer_setting, curr_value;
	struct timespec ts_now, expected;

	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	test_assert_true(timer_fd >= 0);

	timer_setting.it_value.tv_sec = 1;
	timer_setting.it_value.tv_nsec = 0;
	clock_gettime(CLOCK_MONOTONIC, &ts_now);
	timer_setting.it_value = timespec_add(ts_now, timer_setting.it_value);
	timer_setting.it_interval.tv_sec = 0;
	timer_setting.it_interval.tv_nsec = 0;
	timerfd_settime(timer_fd, TFD_TIMER_ABSTIME, &timer_setting, NULL);

	// first gettime: timer not expired yet
	usleep(USEC_PER_SEC / 4);
	timerfd_gettime(timer_fd, &curr_value);

	clock_gettime(CLOCK_MONOTONIC, &ts_now);
	expected = timespec_sub(timer_setting.it_value, ts_now);
	test_assert_true(timespec_approx_equals(&curr_value.it_value, &expected));
	test_assert_true(timespec_is_zero(&curr_value.it_interval));

	// second gettime: timer expired
	usleep(USEC_PER_SEC * 2);
	timerfd_gettime(timer_fd, &curr_value);
	test_assert_true(timespec_is_zero(&curr_value.it_value));
	test_assert_true(timespec_is_zero(&curr_value.it_interval));

	close(timer_fd);
}

TEST_CASE("timerfd set and get (with interval)") {
	int timer_fd = -1;
	struct itimerspec timer_setting, curr_value;

	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	test_assert_true(timer_fd >= 0);

	timer_setting.it_value.tv_sec = 1;
	timer_setting.it_value.tv_nsec = 0;
	timer_setting.it_interval.tv_sec = 0;
	timer_setting.it_interval.tv_nsec = NSEC_PER_SEC / 2;

	timerfd_settime(timer_fd, 0, &timer_setting, NULL);

	// gettime: timer expired once, will expire after the interval
	sleep(1);
	timerfd_gettime(timer_fd, &curr_value);
	test_assert_true(timespec_approx_equals(&curr_value.it_value,
		&timer_setting.it_interval));

	// check that the interval is stored properly in the timerfd
	test_assert_true(timespec_equals(&curr_value.it_interval,
		&timer_setting.it_interval));

	close(timer_fd);
}

TEST_CASE("timerfd set and read (one-time)") {
	int timer_fd = -1;
	struct itimerspec timeout;
	uint64_t expirations_number = 0LL;

	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	test_assert_true(timer_fd >= 0);

	timeout.it_value.tv_sec = 1;
	timeout.it_value.tv_nsec = 0;
	timerfd_settime(timer_fd, 0, &timeout, NULL);

	read(timer_fd, &expirations_number, sizeof(uint64_t)); // blocking
	test_assert_true(expirations_number == 1);
	close(timer_fd);
}

TEST_CASE("timerfd set and read (expired several times)") {
	int timer_fd = -1;
	struct itimerspec timeout;
	uint64_t expirations_number = 0LL;

	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	test_assert_true(timer_fd >= 0);

	timeout.it_value.tv_sec = 0;
	timeout.it_value.tv_nsec = NSEC_PER_SEC / 2;
	timeout.it_interval.tv_sec = 0;
	timeout.it_interval.tv_nsec = NSEC_PER_SEC / 4;
	timerfd_settime(timer_fd, 0, &timeout, NULL);

	sleep(1);
	read(timer_fd, &expirations_number, sizeof(uint64_t));
	test_assert_true(expirations_number == 3);
	close(timer_fd);
}
