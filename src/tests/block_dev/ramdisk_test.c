/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 */

#include <fcntl.h>

#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <embox/test.h>
#include <fs/vfs.h>
#include <mem/page.h>

#include <util/err.h>

EMBOX_TEST_SUITE("fs/ramdisk test");

#define FS_DEV  "/dev/ramdisk"
#define FS_BLOCKS  124

TEST_CASE("Create and delete ramdisk") {
	test_assert_zero(ptr2err(ramdisk_create(FS_DEV, FS_BLOCKS * PAGE_SIZE())));

	test_assert_zero(ramdisk_delete(FS_DEV));
}
