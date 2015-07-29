/**
 * @file
 * @brief Tests for POSIX function select().
 *
 * @date 17.11.13
 * @author Roman Kurbatov
 */

/* TODO: add testing of detection of errors on file descriptors */

#include <stdarg.h>
#include <limits.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("select() tests");

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define READ_END 0
#define WRITE_END 1

#define DELAY 10 /* in milliseconds */
#define SMALL_TIMEOUT 10 /* in milliseconds */
#define BIG_TIMEOUT 10 /* in seconds */

#define PIPES_COUNT 4

static int pipes[PIPES_COUNT][2];
static int max_fd;
static pthread_attr_t attr;

/* Write data to the pipe until it is full. */
static void fill_pipe(const int pipe_fds[]);

/* Read one byte from the pipe after a delay. */
static void *read_from_pipe_after_delay(const int pipe_fds[]);

/* Write one byte to the pipe after a delay. */
static void *write_to_pipe_after_delay(const int pipe_fds[]);

TEST_CASE("descriptor becomes ready to read after a time") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;
	pthread_t thread;

	fill_pipe(pipes[0]);
	fill_pipe(pipes[1]);

	FD_ZERO(&read_fds);
	FD_SET(pipes[2][READ_END], &read_fds);
	FD_SET(pipes[3][READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(pipes[0][WRITE_END], &write_fds);
	FD_SET(pipes[1][WRITE_END], &write_fds);

	timeout.tv_sec = BIG_TIMEOUT;
	timeout.tv_usec = 0;

	pthread_create(&thread, &attr,
			(void *(*)(void *)) &write_to_pipe_after_delay, pipes[2]);

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_equal(ret, 1);
	test_assert_not_zero(FD_ISSET(pipes[2][READ_END], &read_fds));
	test_assert_zero(FD_ISSET(pipes[3][READ_END], &read_fds));
	test_assert_zero(FD_ISSET(pipes[0][WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(pipes[1][WRITE_END], &write_fds));
}

TEST_CASE("descriptor becomes ready to write after a time") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;
	pthread_t thread;

	fill_pipe(pipes[0]);
	fill_pipe(pipes[1]);

	FD_ZERO(&read_fds);
	FD_SET(pipes[2][READ_END], &read_fds);
	FD_SET(pipes[3][READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(pipes[0][WRITE_END], &write_fds);
	FD_SET(pipes[1][WRITE_END], &write_fds);

	timeout.tv_sec = BIG_TIMEOUT;
	timeout.tv_usec = 0;

	pthread_create(&thread, &attr,
			(void *(*)(void *)) &read_from_pipe_after_delay, pipes[0]);

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_equal(ret, 1);
	test_assert_not_zero(FD_ISSET(pipes[0][WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(pipes[1][WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(pipes[2][READ_END], &read_fds));
	test_assert_zero(FD_ISSET(pipes[3][READ_END], &read_fds));
}

TEST_CASE("some of descriptors are ready when select() is "
		"called (zero timeout)") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;
	char buf;

	fill_pipe(pipes[1]);

	buf = 'a';
	ret = write(pipes[2][WRITE_END], &buf, 1);
	test_assert(ret != -1);

	FD_ZERO(&read_fds);
	FD_SET(pipes[2][READ_END], &read_fds);
	FD_SET(pipes[3][READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(pipes[0][WRITE_END], &write_fds);
	FD_SET(pipes[1][WRITE_END], &write_fds);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_equal(ret, 2);
	test_assert_not_zero(FD_ISSET(pipes[0][WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(pipes[1][WRITE_END], &write_fds));
	test_assert_not_zero(FD_ISSET(pipes[2][READ_END], &read_fds));
	test_assert_zero(FD_ISSET(pipes[3][READ_END], &read_fds));
}

TEST_CASE("some of descriptors are ready when select() "
		"is called (no timeout)") {
	fd_set read_fds;
	fd_set write_fds;
	int ret;
	char buf;

	fill_pipe(pipes[1]);

	buf = 'a';
	ret = write(pipes[2][WRITE_END], &buf, 1);
	test_assert(ret != -1);

	FD_ZERO(&read_fds);
	FD_SET(pipes[2][READ_END], &read_fds);
	FD_SET(pipes[3][READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(pipes[0][WRITE_END], &write_fds);
	FD_SET(pipes[1][WRITE_END], &write_fds);

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);

	test_assert_equal(ret, 2);
	test_assert_not_zero(FD_ISSET(pipes[0][WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(pipes[1][WRITE_END], &write_fds));
	test_assert_not_zero(FD_ISSET(pipes[2][READ_END], &read_fds));
	test_assert_zero(FD_ISSET(pipes[3][READ_END], &read_fds));
}

TEST_CASE("no descriptors are ready") {
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	int ret;

	fill_pipe(pipes[0]);
	fill_pipe(pipes[1]);

	FD_ZERO(&read_fds);
	FD_SET(pipes[2][READ_END], &read_fds);
	FD_SET(pipes[3][READ_END], &read_fds);

	FD_ZERO(&write_fds);
	FD_SET(pipes[0][WRITE_END], &write_fds);
	FD_SET(pipes[1][WRITE_END], &write_fds);

	timeout.tv_sec = 0;
	timeout.tv_usec = SMALL_TIMEOUT * 1000;

	ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

	test_assert_zero(ret);
	test_assert_zero(FD_ISSET(pipes[0][WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(pipes[1][WRITE_END], &write_fds));
	test_assert_zero(FD_ISSET(pipes[2][READ_END], &read_fds));
	test_assert_zero(FD_ISSET(pipes[3][READ_END], &read_fds));
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
	int i, j;
	int ret;

	for (i = 0; i < PIPES_COUNT; ++i) {
		ret = pipe(pipes[i]);
		if (ret != 0) {
			return -errno;
		}
	}

	max_fd = INT_MIN;
	for (i = 0; i < PIPES_COUNT; ++i) {
		for (j = 0; j < 2; ++j) {
			if (pipes[i][j] > max_fd) {
				max_fd = pipes[i][j];
			}
		}
	}

	ret = pthread_attr_init(&attr);
	if (ret != 0) {
		return -errno;
	}

	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret != 0) {
		return -errno;
	}

	return 0;
}

static int case_teardown(void) {
	int i, j;
	int ret;

	for (i = 0; i < PIPES_COUNT; ++i) {
		for (j = 0; j < 2; ++j) {
			ret = close(pipes[i][j]);
			if (ret != 0) {
				return -errno;
			}
		}
	}

	ret = pthread_attr_destroy(&attr);
	if (ret != 0) {
		return -errno;
	}

	return 0;
}

static void fill_pipe(const int pipe_fds[]) {
	int flags;
	char buf;
	int ret;

	flags = fcntl(pipe_fds[WRITE_END], F_GETFL);
	flags |= O_NONBLOCK;
	ret = fcntl(pipe_fds[WRITE_END], F_SETFL, flags);
	if(ret == -1) {
		return;
	}

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
	int ret;

	usleep(DELAY * 1000);

	ret = read(pipe_fds[READ_END], &buf, 1);
	test_assert(ret != -1);

	return NULL;
}

static void *write_to_pipe_after_delay(const int pipe_fds[]) {
	char buf;
	int ret;

	usleep(DELAY * 1000);

	buf = 'a';
	ret = write(pipe_fds[WRITE_END], &buf, 1);
	test_assert(ret != -1);

	return NULL;
}
