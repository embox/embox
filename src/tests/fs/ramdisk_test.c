/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 */

#include <fcntl.h>
#include <cmd/mkfs.h>
#include <fs/mount.h>
#include <drivers/ramdisk.h>
#include <embox/test.h>
#include <fs/vfs.h>

static int setup_suite(void);

EMBOX_TEST_SUITE("fs/ramdisk test");

TEST_SETUP(setup_suite);

static mkfs_params_t mkfs_params;

#define FS_NAME  "vfat"
#define FS_DEV  "/dev/ramdisk"
#define FS_TYPE  12
#define FS_BLOCKS  124

TEST_CASE("Create ramdisk") {
	test_assert_zero(ramdisk_create((void *)&mkfs_params));
}

TEST_CASE("Delete ramdisk") {
	test_assert_zero(ramdisk_delete(FS_DEV));
}

static int setup_suite(void) {
	mkfs_params.blocks = FS_BLOCKS;
	mkfs_params.fs_type = FS_TYPE;
	strcpy((void *)&mkfs_params.fs_name, FS_NAME);
	strcpy((void *)&mkfs_params.path, FS_DEV);
	return 0;
}
