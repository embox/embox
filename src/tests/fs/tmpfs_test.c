/**
 * @file
 *
 * @date Dec 20, 2012
 * @author: Felix Sulima
 */


#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fs/tmpfs test");

TEST_SETUP(setup_suite);

TEST_TEARDOWN(teardown_suite);

static const char test_file_filename[] = { "/tmp/tmpfs_test_file.txt" };
static const char test_file_contents[] = { "testing tmpfs\n" };
/* array has a null terminated symbol the size equal size of array except one */
#define SIZE_OF_FILE (sizeof(test_file_contents) - 1)

static int create_test_file(void) {
	int fd;
	int ret;

	if (0 > (fd = creat(test_file_filename, S_IRALL | S_IWALL))) {
		return -1;
	}

	ret = (SIZE_OF_FILE != write(fd, test_file_contents, SIZE_OF_FILE));
	ret |= fsync(fd);
	ret |= close(fd);

	return ret ? -1 : 0;
}

static int remove_test_file(void) {
	return unlink(test_file_filename);
}

TEST_CASE("Attempt to open non-existent file") {
	int fd;

	// remove_test_file(); causes crash
	remove(test_file_filename);
	fd = open(test_file_filename, O_RDONLY);
	test_assert_equal(-1, fd);
	test_assert_equal(ENOENT, errno);
}

TEST_CASE("Read data from a file") {
	char test_buff[SIZE_OF_FILE];
	int fd;

	memset(test_buff, 0, sizeof(test_buff));
	test_assert_zero(create_test_file());

	test_assert(0 <= (fd = open(test_file_filename,O_RDONLY)));
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(close(fd));

	test_assert_zero(remove_test_file());
}

TEST_CASE("Attempt to read more than file length") {
	char test_buff[SIZE_OF_FILE + 1];
	int fd;

	memset(test_buff, 0, sizeof(test_buff));
	test_assert_zero(create_test_file());

	test_assert(0 <= (fd = open(test_file_filename,O_RDONLY)));
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE + 1));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(close(fd));

	test_assert_zero(remove_test_file());
}

TEST_CASE("Call stat for a file") {
	struct stat stat_buff;

	test_assert_zero(create_test_file());

	test_assert_zero(stat(test_file_filename, &stat_buff));
	test_assert_equal(SIZE_OF_FILE, stat_buff.st_size);

	test_assert_zero(remove_test_file());
}

TEST_CASE("Call fstat for a file") {
	struct stat stat_buff;
	int fd;

	test_assert_zero(create_test_file());

	test_assert(0 <= (fd = open(test_file_filename, O_RDONLY)));
	test_assert_zero(fstat(fd, &stat_buff));
	test_assert_equal(SIZE_OF_FILE, stat_buff.st_size);

	test_assert_zero(remove_test_file());
}

/* TODO - not implemented
 * https://www.securecoding.cert.org/confluence/display/seccode/FIO08-C.+Take+care+when+calling+remove()+on+an+open+file
TEST_CASE("Read from removed open file") {
	char test_buff[SIZE_OF_FILE];
	struct stat stat_buff;
	int fd;

	test_assert_zero(create_test_file());

	test_assert(0 <= (fd = open(test_file_filename, O_RDONLY)));
	test_assert_zero(remove_test_file());
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(close(fd));

	if (0 == stat(test_file_filename, &stat_buff)) {
		test_assert(!S_ISREG(stat_buff.st_mode));
	}
}
*/

TEST_CASE("Test append") {
	char test_buff[SIZE_OF_FILE];
	int fd;

	memset(test_buff, 0, sizeof(test_buff));
	test_assert_zero(create_test_file());

	test_assert(0 <= (fd = open(test_file_filename,O_WRONLY|O_APPEND)));
	test_assert(SIZE_OF_FILE == write(fd, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(close(fd));
	test_assert(0 <= (fd = open(test_file_filename,O_RDONLY)));
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(close(fd));

	test_assert_zero(remove_test_file());
}

TEST_CASE("Test truncate") {
	char test_buff[SIZE_OF_FILE];
	int fd;
	int ret;

	memset(test_buff, 0, sizeof(test_buff));
	test_assert_zero(create_test_file());

	test_assert(0 <= (fd = open(test_file_filename,O_WRONLY|O_TRUNC)));
	ret = lseek(fd, 0, SEEK_END);
	test_assert_equal(0, ret);
	test_assert(SIZE_OF_FILE == write(fd, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(close(fd));
	test_assert(0 <= (fd = open(test_file_filename,O_RDONLY)));
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(close(fd));

	test_assert_zero(remove_test_file());
}

TEST_CASE("Test lseek") {
	char test_buff[SIZE_OF_FILE];
	int fd;

	memset(test_buff, 0, sizeof(test_buff));
	test_assert_zero(create_test_file());

	test_assert(0 <= (fd = open(test_file_filename,O_RDONLY)));
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_equal(1, lseek(fd, 1, SEEK_SET));
	test_assert_equal(SIZE_OF_FILE-1, read(fd, &test_buff[1], SIZE_OF_FILE-1));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(lseek(fd, -SIZE_OF_FILE, SEEK_CUR));
	test_assert_equal(SIZE_OF_FILE, read(fd, test_buff, SIZE_OF_FILE));
	test_assert_zero(strncmp(test_buff, test_file_contents, SIZE_OF_FILE));
	test_assert_zero(close(fd));

	test_assert_zero(remove_test_file());
}

TEST_CASE("Test fcntl") {
}

static int setup_suite(void) {
	return 0;
}

static int teardown_suite(void) {
	return 0;
}
