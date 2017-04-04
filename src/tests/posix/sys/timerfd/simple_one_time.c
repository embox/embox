/**
 * @file
 * @brief A simple test for timerfd.
 * @details Sets the timer and calls read() on it once.
 *
 * @date 04.04.17
 * @author Kirill Smirenko
 */

#include <embox/test.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/timerfd.h>

EMBOX_TEST_SUITE("timerfd suite");

TEST_CASE("simple timerfd test") {
	int timeout_sec = 1;

	int error_code;
	int timerfd = -1;
	struct itimerspec timeout;
	uint64_t expirations_number;

	/* create new timer */
	timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
	test_assert_not_zero(timerfd);

	/* set timeout */
	timeout.it_value.tv_sec = timeout_sec;
	timeout.it_value.tv_nsec = 0;
	error_code = timerfd_settime(timerfd, 0, &timeout, NULL);
	test_assert_zero(error_code);

	read(timerfd, &expirations_number, sizeof(expirations_number));
	test_assert(expirations_number == 1);
}
