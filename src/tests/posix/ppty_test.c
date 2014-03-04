/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    19.11.2013
 */

#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <embox/test.h>

extern int ppty(int pptyfds[2]);

EMBOX_TEST_SUITE("ppty() testsuite");

TEST_SETUP(ppty_test_setup);
TEST_TEARDOWN(ppty_test_teardown);

static int ppty_testfd[2];
static int fdm, fds;

static int ppty_test_orfl(int fd, int flags) {
	int res;

	res = fcntl(fd, F_GETFL);
	if (res == -1) {
		return res;
	}

	res |= O_NONBLOCK;
	res = fcntl(fd, F_SETFL, res);
	if (res == -1) {
		return res;
	}

	return 0;
}

int ppty_test_setup(void) {
	int res;

	res = ppty(ppty_testfd);
	if (res) {
		return res;
	}

	if (-1 == ppty_test_orfl(ppty_testfd[0], O_NONBLOCK)) {
		return -errno;
	}

	if (-1 == ppty_test_orfl(ppty_testfd[1], O_NONBLOCK)) {
		return -errno;
	}

	fdm = ppty_testfd[0];
	fds = ppty_testfd[1];

	return 0;
}

int ppty_test_teardown(void) {

	close(ppty_testfd[0]);
	close(ppty_testfd[1]);

	return 0;
}

static int writestr(int fd, const char *str) {
	int len = strlen(str);

	return len != write(fd, str, len);
}

#define READSTR_BUFLEN 16
static int readstr(int fd, const char *str) {
	static char readstr_buf[READSTR_BUFLEN];
	int len = strlen(str);
	int res;

	if (len != (res = read(fd, readstr_buf, len))) {
		return res;
	}

	if ((res = strncmp(readstr_buf, str, len))) {
		return res;
	}

	return 0;
}

static int tcsetclearattr(int fd, int and, int or) {
	struct termios tio;
	int res;

	res = tcgetattr(fd, &tio);
	if (res != 0) {
		return res;
	}

	tio.c_lflag &= and;
	tio.c_lflag |= or;

	return tcsetattr(fd, TCSAFLUSH, &tio);
}

TEST_CASE("termios should be able to get and set") {
	struct termios tio;

	test_assert_zero(tcgetattr(fds, &tio));

	test_assert_true(tio.c_lflag & ICANON);

	tio.c_lflag &= ~ICANON;

	test_assert_zero(tcsetattr(fds, TCSAFLUSH, &tio));

	memset(&tio, 0, sizeof(struct termios));

	test_assert_zero(tcgetattr(fds, &tio));

	test_assert_false(tio.c_lflag & ICANON);
}

TEST_CASE("In canonical mode master shouldn't accept slave input immediately") {

	test_assert_zero(tcsetclearattr(fds, ~0, ICANON));

	test_assert_zero(writestr(fdm, "abc"));

	test_assert_equal(-1, readstr(fds, "abc"));
	test_assert_equal(EAGAIN, errno);

	test_assert_zero(writestr(fdm, "d\n"));

	test_assert_equal(0, readstr(fds, "abcd\n"));
}

TEST_CASE("Slave tty should be a tty (test by isatty())") {

	test_assert_true(isatty(fds));
}
