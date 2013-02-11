/**
 * @file
 *
 * @date Nov 28, 2012
 * @author: Anton Bondarev
 */


#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fs/initfs test");

TEST_SETUP(setup_suite);

TEST_TEARDOWN(teardown_suite);

#define TEST_FILE_NAME "/initfs_test_file.txt"

static int test_fd;
static const char test_file_contains[] = { "\""
      #include "initfs_test_file.txt"
		"\""
};
/* array has a null terminated symbol the size equal size of array except one */
#define SIZE_OF_FILE (sizeof(test_file_contains) - 1)

static char test_buff[SIZE_OF_FILE + 1 + 1]; /* enough size for read overhead data */

TEST_CASE("Write data to a read only file on a initfs file system") {
	test_assert_equal(-1, write(test_fd, test_buff, 1));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("Read data from a file") {
	test_assert_equal(SIZE_OF_FILE, read(test_fd, test_buff, SIZE_OF_FILE));
	/* we don't compare null terminate simbol */
	test_assert_zero(strncmp(test_buff, test_file_contains, SIZE_OF_FILE));
}

TEST_CASE("Try to read data quantity more then contains in the file") {
	test_assert_equal(SIZE_OF_FILE, read(test_fd, test_buff, SIZE_OF_FILE + 1));
	test_assert_zero(strncmp(test_buff, test_file_contains, sizeof(test_file_contains)));
}


TEST_CASE("Call stat from a file") {
	struct stat stat_buff;

	test_assert_zero(stat(TEST_FILE_NAME, &stat_buff));
	test_assert_equal(SIZE_OF_FILE, stat_buff.st_size);
}

TEST_CASE("Call fstat from a file") {
	struct stat stat_buff;

	test_assert_zero(fstat(test_fd, &stat_buff));
	test_assert_equal(SIZE_OF_FILE, stat_buff.st_size);
}

TEST_CASE("Try to remove file from initfs") {
	errno = ENOERR;
	test_assert_equal(-1, unlink(TEST_FILE_NAME));
	test_assert_equal(EPERM, errno);
	errno = ENOERR;
	test_assert_equal(-1, remove(TEST_FILE_NAME));
	test_assert_equal(EPERM, errno);
}

TEST_CASE("Call fcntl") {

}

static int setup_suite(void) {
	memset(test_buff, 0, sizeof(test_buff));
	test_assert(0 <= (test_fd = open(TEST_FILE_NAME, O_RDONLY)));
	return 0;
}

static int teardown_suite(void) {
	test_assert_zero(close(test_fd));
	return 0;
}
