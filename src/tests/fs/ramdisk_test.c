/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 */

#include <fcntl.h>
#include <cmd/mkfs.h>
#include <drivers/ramdisk.h>
#include <embox/test.h>
#include <fs/vfs.h>
#include <mem/page.h>

EMBOX_TEST_SUITE("fs/ramdisk test");

#define FS_DEV  "/dev/ramdisk"
#define FS_BLOCKS  124

TEST_CASE("Create ramdisk") {
	test_assert_zero(ramdisk_create(FS_DEV, FS_BLOCKS * PAGE_SIZE()));
}

TEST_CASE("Delete ramdisk") {
	test_assert_zero(ramdisk_delete(FS_DEV));
}

