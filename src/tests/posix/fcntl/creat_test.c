/**
 * @file
 *
 * @date Oct 30, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <unistd.h>

#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <embox/test.h>
#include <fs/fsop.h>
#include <util/err.h>

EMBOX_TEST_SUITE("posix/fctl/creat test");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME    "vfat"
#define FS_DEV     "/dev/ram_test"
#define FS_DIR     "/tmp"
#define MKDIR_PERM 0700

#define FS_BLOCKS  124

static int setup_suite(void) {
	int res;

	res = ptr2err(ramdisk_create(FS_DEV, FS_BLOCKS * sysconf(_SC_PAGESIZE)));
	if (res != 0) {
		return res;
	}

	/* format filesystem */
	if (0 != (res = format(FS_DEV, FS_NAME))) {
		return res;
	}

	/*	if (0 != mkdir(FS_DIR, MKDIR_PERM)) {
		return -errno;
	}
*/
	/* mount filesystem */
	if (-1 == mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL)) {
		return -errno;
	}

	return 0;
}

static int teardown_suite(void) {
	int res;

	if (0 != (res = umount(FS_DIR))) {
		return res;
	}

	/*	if (0 != rmdir(FS_DIR)) {
		return res;
	}
*/
	if (0 != (res = ramdisk_delete(FS_DEV))) {
		return res;
	}

	return 0;
}
