/**
 * @file
 *
 * @date Oct 30, 2013
 * @author:
 */
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fcntl/open");

#define TEST_FILE_NAME          "testfile.txt"
#define ABS_PATH_TEST_FILE_NAME "/"TEST_FILE_NAME
#define TOO_LONG_FILE_NAME      \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"1111111111" \
								"88888888.txt"

TEST_CASE("open an existing file") {
	int fd;

	fd = open(TEST_FILE_NAME, O_RDONLY);
	test_assert(fd != -1);
	close(fd);
}

TEST_CASE("open an existing file by absolute path") {
	int fd;

	fd = open(ABS_PATH_TEST_FILE_NAME, O_RDONLY);
	test_assert(fd != -1);
	close(fd);
}

TEST_CASE("open not existing with too long file name") {
	int fd;

	fd = open(TOO_LONG_FILE_NAME, O_RDONLY);
	test_assert(fd == -1);
	test_assert(errno == ENAMETOOLONG);
}
