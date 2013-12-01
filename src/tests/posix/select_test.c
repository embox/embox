/**
 * @file
 * @brief Tests for POSIX function select().
 *
 * @date 17.11.13
 * @author Roman Kurbatov
 */

#include <stdarg.h>
#include <limits.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>

#include <kernel/thread.h>
#include <kernel/time/ktime.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("select() tests");

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define READ_END 0
#define WRITE_END 1

#define DELAY 100 /* in milliseconds */
#define SMALL_TIMEOUT 10 /* in milliseconds */
#define BIG_TIMEOUT 10 /* in seconds */

static int first_pipe[2], second_pipe[2], third_pipe[2], fourth_pipe[2];
static int max_fd;

/* Write data to the pipe until it is full. */
static void fill_pipe(const int pipe_fds[]);

/* Read one byte from the pipe after a delay. */
static void *read_from_pipe_after_delay(const int pipe_fds[]);

/* Write one byte to the pipe after a delay. */
static void *write_to_pipe_after_delay(const int pipe_fds[]);

/* Get the maximum of several int numbers. */
static int max(int count, ...);

TEST_CASE("descriptor becomes ready to read after a time") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;

	fill_pipe(first_pipe);
	fill_pipe(second_pipe);

	FD_ZERO(&read_fds);
	FD_SET(third_pipe[READ_END], &read_fds);
	FD_SET(fourth_pipe[READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(first_pipe[WRITE_END], &write_fds);
	FD_SET(second_pipe[WRITE_END], &write_fds);

	timeout.tv_sec = BIG_TIMEOUT;
	timeout.tv_usec = 0;

	thread_create(0, (void *(*)(void *)) &write_to_pipe_after_delay, third_pipe);

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_equal(ret, 1);
	test_assert_not_zero(FD_ISSET(third_pipe[READ_END], &read_fds));
	test_assert_zero(FD_ISSET(fourth_pipe[READ_END], &read_fds));
	test_assert_zero(FD_ISSET(first_pipe[WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(second_pipe[WRITE_END], &write_fds));
}

TEST_CASE("descriptor becomes ready to write after a time") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;

	fill_pipe(first_pipe);
	fill_pipe(second_pipe);

	FD_ZERO(&read_fds);
	FD_SET(third_pipe[READ_END], &read_fds);
	FD_SET(fourth_pipe[READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(first_pipe[WRITE_END], &write_fds);
	FD_SET(second_pipe[WRITE_END], &write_fds);

	timeout.tv_sec = BIG_TIMEOUT;
	timeout.tv_usec = 0;

	thread_create(0, (void *(*)(void *)) &read_from_pipe_after_delay, first_pipe);

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_equal(ret, 1);
	test_assert_not_zero(FD_ISSET(first_pipe[WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(second_pipe[WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(third_pipe[READ_END], &read_fds));
	test_assert_zero(FD_ISSET(fourth_pipe[READ_END], &read_fds));
}

TEST_CASE("some of descriptors are ready when select() is "
		"called (zero timeout)") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;
	char buf;

	fill_pipe(second_pipe);

	buf = 'a';
	write(third_pipe[WRITE_END], &buf, 1);

	FD_ZERO(&read_fds);
	FD_SET(third_pipe[READ_END], &read_fds);
	FD_SET(fourth_pipe[READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(first_pipe[WRITE_END], &write_fds);
	FD_SET(second_pipe[WRITE_END], &write_fds);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_equal(ret, 2);
	test_assert_not_zero(FD_ISSET(first_pipe[WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(second_pipe[WRITE_END], &write_fds));
	test_assert_not_zero(FD_ISSET(third_pipe[READ_END], &read_fds));
	test_assert_zero(FD_ISSET(fourth_pipe[READ_END], &read_fds));
}

TEST_CASE("some of descriptors are ready when select() "
		"is called (no timeout)") {
	fd_set read_fds;
	fd_set write_fds;
	int ret;
	char buf;

	fill_pipe(second_pipe);

	buf = 'a';
	write(third_pipe[WRITE_END], &buf, 1);

	FD_ZERO(&read_fds);
	FD_SET(third_pipe[READ_END], &read_fds);
	FD_SET(fourth_pipe[READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(first_pipe[WRITE_END], &write_fds);
	FD_SET(second_pipe[WRITE_END], &write_fds);

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);

	test_assert_equal(ret, 2);
	test_assert_not_zero(FD_ISSET(first_pipe[WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(second_pipe[WRITE_END], &write_fds));
	test_assert_not_zero(FD_ISSET(third_pipe[READ_END], &read_fds));
	test_assert_zero(FD_ISSET(fourth_pipe[READ_END], &read_fds));
}

TEST_CASE("no descriptors are ready") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;

	fill_pipe(first_pipe);
	fill_pipe(second_pipe);

	FD_ZERO(&read_fds);
	FD_SET(third_pipe[READ_END], &read_fds);
	FD_SET(fourth_pipe[READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(first_pipe[WRITE_END], &write_fds);
	FD_SET(second_pipe[WRITE_END], &write_fds);

	timeout.tv_sec = 0;
	timeout.tv_usec = SMALL_TIMEOUT * 1000;

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_zero(ret);
	test_assert_zero(FD_ISSET(first_pipe[WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(second_pipe[WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(third_pipe[READ_END], &read_fds));
	test_assert_zero(FD_ISSET(fourth_pipe[READ_END], &read_fds));
}

TEST_CASE("no sets of descriptors are given") {
	struct timeval timeout;
	int ret;

	timeout.tv_sec = 0;
	timeout.tv_usec = SMALL_TIMEOUT;

	ret = select(0, NULL, NULL, NULL, &timeout);
	test_assert_zero(ret);
}

static int case_setup(void) {
	pipe(first_pipe);
	pipe(second_pipe);
	pipe(third_pipe);
	pipe(fourth_pipe);

	max_fd = max(8, first_pipe[READ_END], first_pipe[WRITE_END],
			second_pipe[READ_END], second_pipe[WRITE_END],
			third_pipe[READ_END], third_pipe[WRITE_END],
			fourth_pipe[READ_END], fourth_pipe[WRITE_END]);

	return 0;
}

static int case_teardown(void) {
	close(first_pipe[READ_END]);
	close(first_pipe[WRITE_END]);

	close(second_pipe[READ_END]);
	close(second_pipe[WRITE_END]);

	close(third_pipe[READ_END]);
	close(third_pipe[WRITE_END]);

	close(fourth_pipe[READ_END]);
	close(fourth_pipe[WRITE_END]);

	return 0;
}

static void fill_pipe(const int pipe_fds[]) {
	int flags;
	char buf;
	int ret;

	flags = fcntl(pipe_fds[WRITE_END], F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(pipe_fds[WRITE_END], F_SETFL, flags);

	buf = 'a';
	while (1) {
		ret = write(pipe_fds[WRITE_END], &buf, 1);
		if (ret <= 0) {
			break;
		}
	}
}

static void *read_from_pipe_after_delay(const int pipe_fds[]) {
	char buf;

	ksleep(DELAY);

	read(pipe_fds[READ_END], &buf, 1);

	return NULL;
}

static void *write_to_pipe_after_delay(const int pipe_fds[]) {
	char buf;

	ksleep(DELAY);

	buf = 'a';
	write(pipe_fds[WRITE_END], &buf, 1);

	return NULL;
}

static int max(int count, ...) {
	va_list ap;
	int res;
	int n;

	va_start(ap, count);

	res = INT_MIN;
	while (count-- > 0) {
		n = va_arg(ap, int);
		if (n > res) {
			res = n;
		}
	}

	va_end(ap);

	return res;
}
