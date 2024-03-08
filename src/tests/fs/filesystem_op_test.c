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


#include <fs/vfs.h>
#include <fs/fs_driver.h>
#include <fs/mount.h>
#include <fs/fsop.h>

#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <drivers/block_dev.h>
#include <embox/test.h>

#include <mem/page.h>

#include <util/err.h>

EMBOX_TEST_SUITE("fs/filesystem test");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME  "vfat"
#define FS_DEV  "/dev/ramdisk"
#define FS_BLOCKS  124
#define FS_DIR    "/tmp"
#define FS_DIR1   "/tmp/foo"
#define FS_DIR2   "/tmp/foo/bar"
#define FS_FILE1  "/tmp/foo/bar/abc.txt"
#define FS_FILE2  "/tmp/foo/bar/xyz.txt"

TEST_CASE("generic filesystem test") {

	/* Create fat filesystem */
	{
		test_assert_zero(format(FS_DEV, FS_NAME));
	}

	/* Mount fat filesystem */
	{
		test_assert_zero(mount(FS_DEV, FS_DIR, FS_NAME));
	}

	/* Create files and directories */
	{
		int fd;
		test_assert_zero(mkdir(FS_DIR1, 0777));
		test_assert_zero(mkdir(FS_DIR2, 0777));
		test_assert_true(0 <= (fd = creat(FS_FILE1, 0)));
		test_assert_not_equal(-1, fd);
		close(fd);
		test_assert_true(0 <= (fd = creat(FS_FILE2, 0)));
		test_assert_not_equal(-1, fd);
		close(fd);
	}

	/* Delete file */
	{
		test_assert_zero(remove(FS_FILE1));
		test_assert_zero(remove(FS_FILE2));
		test_assert(0 > open(FS_FILE1, O_RDONLY));
		test_assert(0 > open(FS_FILE2, O_RDONLY));
		test_assert_zero(remove(FS_DIR2));
		test_assert_zero(remove(FS_DIR1));
	}

	/* Umount fat filesystem */
	{
		test_assert_zero(umount(FS_DIR));
	}

}

static int setup_suite(void) {
	int res;

	if(0 != (res = ptr2err(ramdisk_create(FS_DEV, FS_BLOCKS * PAGE_SIZE())))) {
		return res;
	}

	return 0;
}

static int teardown_suite(void) {
	return ramdisk_delete(FS_DEV);
}
