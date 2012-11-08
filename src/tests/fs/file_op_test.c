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
#include <mem/page.h>
#include <embox/test.h>
#include <embox/block_dev.h>
#include <fs/vfs.h>

EMBOX_TEST_SUITE("fs/file test");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

static mkfs_params_t mkfs_params;
static dev_ramdisk_t *ramdisk;
static mount_params_t mount_param;
static fs_drv_t *fs_drv;

#define FS_NAME  "vfat"
#define FS_DEV  "/dev/ramdisk"
#define FS_TYPE  12
#define FS_BLOCKS  124
#define FS_DIR  "/test_fop"
#define FS_FILE1  "/test_fop/1/2/3/1.txt"
#define FS_DIR3  "/test_fop/1/2/3"
#define FS_DIR2  "/test_fop/1/2"
#define FS_DIR1  "/test_fop/1"

#define FS_TESTDATA  "qwerty\n"
TEST_CASE("Write file") {
	int file;

	test_assert(0 <=  (file = open(FS_FILE1, O_WRONLY)));
	test_assert_zero(lseek(file, 0, SEEK_END));
	test_assert(0 < write(file, FS_TESTDATA, strlen(FS_TESTDATA)));
	test_assert_zero(close(file));
}

#define FS_FILE2  "/test_fop/1/2/3/2.txt"
TEST_CASE("Copy file") {
	int src_file, dst_file;
	char buf[PAGE_SIZE()];
	int bytesread;

	test_assert(0 <=  (src_file = open(FS_FILE1, O_WRONLY)));
	test_assert(0 <=  (dst_file = open(FS_FILE2, O_WRONLY)));
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

	test_assert(0 <=  (file = open(FS_FILE2, O_RDONLY)));
	test_assert_zero(lseek(file, 0, SEEK_SET));

	test_assert(0 <= read(file, buf, PAGE_SIZE()));
	test_assert_zero(strcmp(FS_TESTDATA, buf));

	test_assert_zero(close(file));
}


static int setup_suite(void) {

	mkfs_params.blocks = FS_BLOCKS;
	mkfs_params.fs_type = FS_TYPE;
	strcpy((void *)&mkfs_params.fs_name, FS_NAME);
	strcpy((void *)&mkfs_params.path, FS_DEV);

	if (0 != ramdisk_create((void *)&mkfs_params)) {
		return -1;
	}

	if(NULL == (fs_drv =
			filesystem_find_drv((const char *) &mkfs_params.fs_name))) {
		return -1;
	}

	mount_param.dev = FS_DEV;
	mount_param.dir = FS_DIR;

	if(NULL ==	(mount_param.dev_node =
			vfs_find_node(mount_param.dev, NULL))) {
		return -1;
	}
	/* set created ramdisc attribute from dev_node */
	ramdisk = (dev_ramdisk_t *) block_dev(mount_param.dev_node->dev_id)->privdata;

	/* format filesystem */
	if(0 != fs_drv->fsop->format((void *)&ramdisk->path)) {
		return -1;
	}

	/* mount filesystem */
	if(fs_drv->fsop->mount((void *) &mount_param)) {
		return -1;
	}

	return create(FS_FILE1, 0);
}

static int teardown_suite(void) {

	if (remove(FS_FILE1) ||	remove(FS_FILE2) ||
		remove(FS_DIR3)  ||	remove(FS_DIR2)  ||
		remove(FS_DIR1)  ||	remove(FS_DIR)) {
		return -1;
	}
	if (ramdisk_delete(FS_DEV)) {
		return -1;
	}
	return 0;
}
