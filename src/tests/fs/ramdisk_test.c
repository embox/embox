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
#include <mem/page.h>

static int setup_suite(void);

EMBOX_TEST_SUITE("fs/ramdisk test");

TEST_SETUP(setup_suite);

static ramdisk_create_params_t new_ramdisk;

#define FS_NAME  "vfat"
#define FS_DEV  "/dev/ramdisk"
#define FS_TYPE  12
#define FS_BLOCKS  124

TEST_CASE("Create ramdisk") {
	test_assert_zero(ramdisk_create((void *)&new_ramdisk));
}

TEST_CASE("Delete ramdisk") {
	test_assert_zero(ramdisk_delete(FS_DEV));
}

static int setup_suite(void) {
	new_ramdisk.size = FS_BLOCKS * PAGE_SIZE();
	new_ramdisk.fs_type = FS_TYPE;
	new_ramdisk.fs_name = FS_NAME;
	new_ramdisk.path = FS_DEV;
	return 0;
}
