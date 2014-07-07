/**
 * @file
 * @brief
 *
 * @date 21.10.13
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>

EMBOX_TEST_SUITE("poll tests");

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define BAD_FD  10
#define TIMEOUT 11

static int fildes[2];
static struct pollfd fds_r, fds_w;
static const nfds_t nfds = 1;

TEST_CASE("poll() fails on invalid desccriptor") {
	fds_r.fd = BAD_FD;
	test_assert_equal(1, poll(&fds_r, nfds, 0));
	test_assert_equal(POLLNVAL, fds_r.revents);
}

TEST_CASE("poll() ignores negative descriptor") {
	fds_r.fd = -BAD_FD;
	test_assert_equal(0, poll(&fds_r, nfds, 0));
	test_assert_equal(0, fds_r.revents);
}

TEST_CASE("poll() returns 0 if no events occurred") {
	test_assert_equal(0, poll(&fds_r, nfds, 0));
}

TEST_CASE("poll() wait at least timeout and returns 0"
		" if no events occurred") {
	test_assert_equal(0, poll(&fds_r, nfds, TIMEOUT));
}

TEST_CASE("poll() clears revents field") {
	test_assert_equal(0, poll(&fds_r, nfds, 0));
	test_assert_equal(0, fds_r.revents);
}

TEST_CASE("poll() catches POLLIN event") {
	test_assert_equal(1, write(fds_w.fd, "a", 1));
	test_assert_equal(1, poll(&fds_r, nfds, 0));
	test_assert_equal(POLLIN, fds_r.revents);

	test_assert_equal(1, write(fds_w.fd, "a", 1));
	test_assert_equal(1, poll(&fds_r, nfds, TIMEOUT));
	test_assert_equal(POLLIN, fds_r.revents);
}

TEST_CASE("poll() catches POLLOUT event") {
	test_assert_equal(1, poll(&fds_w, nfds, 0));
	test_assert_equal(POLLOUT, fds_w.revents);

	test_assert_equal(1, poll(&fds_w, nfds, TIMEOUT));
	test_assert_equal(POLLOUT, fds_w.revents);
}

TEST_CASE("poll() ignores any events if the events field is 0") {
	fds_r.events = 0;
	test_assert_equal(1, write(fds_w.fd, "a", 1));
	test_assert_equal(0, poll(&fds_r, nfds, 0));
	test_assert_equal(0, fds_r.revents);

	fds_w.events = 0;
	test_assert_equal(0, poll(&fds_w, nfds, 0));
	test_assert_equal(0, fds_w.revents);
}

TEST_CASE("poll() accept 2 descriptors and monitors one of them") {
	struct pollfd fds[2];
	int fds_cnt = 2;

	fds[0].fd = fds_w.fd;
	fds[1].fd = fds_r.fd;
	fds[0].events = 0; // don't monitor
	fds[1].events = POLLIN; // monitor
	fds[0].revents = fds[1].revents = 0;

	test_assert_equal(1, write(fds_w.fd, "a", 1));
	test_assert_equal(1, poll(fds, fds_cnt, 0));
	test_assert_equal(POLLIN, fds[1].revents);
}

static int case_setup(void) {
	if (-1 == pipe(fildes)) {
		return -errno;
	}

	fds_r.fd = fildes[0];
	fds_w.fd = fildes[1];
	fds_r.events = fds_w.events = POLLIN | POLLOUT;
	fds_r.revents = fds_w.revents = ~0;

	return 0;
}

static int case_teardown(void) {
	if (-1 == close(fildes[0])) {
		return -errno;
	}
	if (-1 == close(fildes[1])) {
		return -errno;
	}
	return 0;
}
