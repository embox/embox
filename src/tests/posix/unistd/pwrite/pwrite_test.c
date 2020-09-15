/**
 * @file
 *
 * @date Sep 14, 2020
 * @author Anton Bondarev
 */

#include <fcntl.h>
#include <unistd.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("fs write tests");

#define FS_TEST_PWRITE_FILE "/mnt/fs_test/fs_pwrite_test"
#define TEST_STR_5678 "5678"
#define TEST_STR_1234 "1234"

TEST_CASE("pwrite test") {
	int fd;
	char buf[10];
	int len;

	fd = creat(FS_TEST_PWRITE_FILE, 0);

	pwrite(fd, TEST_STR_5678, sizeof(TEST_STR_5678), 0);
	pwrite(fd, TEST_STR_1234, sizeof(TEST_STR_1234), 0);

	len = pread(fd, buf, sizeof(buf), 0);

	test_assert_equal(len, 8);
	test_assert_zero(memcmp(buf, TEST_STR_1234 TEST_STR_5678, len));

	close(fd);

	remove(FS_TEST_PWRITE_FILE);
}
