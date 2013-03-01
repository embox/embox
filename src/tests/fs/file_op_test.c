/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <fs/vfs.h>
#include <fs/fs_driver.h>
#include <fs/sys/fsop.h>
#include <embox/block_dev.h>
#include <drivers/ramdisk.h>

#include <mem/page.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("fs/file test");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME  "vfat"
#define FS_DEV  "/dev/ramdisk"
#define FS_DIR  "/tmp"
#define FS_TYPE  12
#define FS_BLOCKS  124

#define FS_FILE1  "/tmp/1/2/3/1.txt"
#define FS_FILE2  "/tmp/1/2/3/2.txt"
#define FS_DIR3  "/tmp/1/2/3"
#define FS_DIR2  "/tmp/1/2"
#define FS_DIR1  "/tmp/1"
#define FS_TESTDATA  "qwerty\n"

TEST_CASE("Write file") {
	int file;

	test_assert(0 <= (file = open(FS_FILE1, O_WRONLY)));
	test_assert_zero(lseek(file, 0, SEEK_END));
	test_assert(0 < write(file, FS_TESTDATA, strlen(FS_TESTDATA)));
	test_assert_zero(close(file));
}

TEST_CASE("Copy file") {
	int src_file, dst_file;
	char buf[PAGE_SIZE()];
	int bytesread;

	test_assert(0 <=  (src_file = open(FS_FILE1, O_RDONLY)));
	test_assert(0 <=  (dst_file = open(FS_FILE2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)));
	test_assert_zero(lseek(dst_file, 0, SEEK_SET));

	bytesread = 0;
	while (1) {
		test_assert(0 <=  (bytesread = read(src_file, buf, PAGE_SIZE())));
		if(0 >= bytesread) {
			break;
		}
		test_assert(0 < write (dst_file, buf, bytesread));
	}

	test_assert_zero(close(src_file));
	test_assert_zero(close(dst_file));
}

TEST_CASE("Read file") {
	int file;
	char buf[PAGE_SIZE()];

	memset(buf, 0, PAGE_SIZE());

	test_assert(0 <=  (file = open(FS_FILE2, O_RDONLY, S_IRUSR)));
	test_assert_zero(lseek(file, 0, SEEK_SET));

	test_assert_equal(strlen(FS_TESTDATA), read(file, buf, PAGE_SIZE()));
	test_assert_zero(strcmp(FS_TESTDATA, buf));

	test_assert_zero(close(file));
}

/*
TEST_CASE("stat and fstat should return same stats") {
	struct stat st, fst;
	int fd;

	stat(FS_FILE2, &st);

	test_assert((fd = open(FS_FILE2, O_RDONLY)) >= 0);

	fstat(fd, &fst);

	test_assert(0 == memcmp(&st, &fst, sizeof(struct stat)));

	close(fd);
}
*/

#define MKDIR_PERM 0700

static int setup_suite(void) {
	int fd, res;

	if (0 != ramdisk_create(FS_DEV, FS_BLOCKS * PAGE_SIZE())) {
		return -1;
	}

	/* format filesystem */
	if (0 != (res = format(FS_DEV, FS_NAME))) {
		return res;
	}

	/* mount filesystem */
	if (0 != (res = mount(FS_DEV, FS_DIR, FS_NAME))) {
		return res;
	}

	if (0 != (res = mkdir(FS_DIR1, MKDIR_PERM))) {
		return res;
	}

	if (0 != (res = mkdir(FS_DIR2, MKDIR_PERM))) {
		return res;
	}

	if (0 != (res = mkdir(FS_DIR3, 0777))) {
		return res;
	}

	if (-1 == (fd = creat(FS_FILE1, S_IRUSR | S_IWUSR))) {
		return -errno;
	}

	close(fd);

	return 0;
}

static int teardown_suite(void) {

	if (remove(FS_FILE1) ||	remove(FS_FILE2) ||
		remove(FS_DIR3)  ||	remove(FS_DIR2)  ||
		remove(FS_DIR1)) {
		return -1;
	}
	if (umount(FS_DIR) || ramdisk_delete(FS_DEV)) {
		return -1;
	}
	return 0;
}
