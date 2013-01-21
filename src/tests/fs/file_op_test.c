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
#include <fs/fs_drv.h>
#include <fs/sys/fsop.h>/* now mount declaration in this header */
#include <embox/block_dev.h>
#include <drivers/ramdisk.h>

#include <mem/page.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("fs/file test");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);


#define FS_NAME			"vfat"
#define FS_DEV			"/dev/ramdisk"
#define FS_TYPE			12
#define FS_BLOCKS		124
#define FS_DIR			"/test_fop"
#define FS_FILE1		"/test_fop/1/2/3/1.txt"
#define FS_FILE1_NAME	"1.txt"
#define FS_FILE2		"/test_fop/1/2/3/2.txt"
#define FS_FILE3		"/test_fop/1/2/3/renamed_file"
#define FS_FILE3_NAME	"renamed_file"
#define FS_DIR3			"/test_fop/1/2/3"
#define FS_DIR2			"/test_fop/1/2"
#define FS_DIR1			"/test_fop/1"
#define FS_TESTDATA		"qwerty\n"
#define FS_TOOLONGNAME	"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnam"

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

TEST_CASE("Rename file") {
	test_assert(-1 == rename("no_such_file", FS_FILE3_NAME));
	test_assert(EINVAL == errno);
	test_assert(-1 == rename(FS_FILE1, FS_FILE2));
	test_assert(EINVAL == errno);
	test_assert(-1 == rename(FS_TOOLONGNAME, "no_matter"));
	test_assert(ENAMETOOLONG == errno);
	test_assert(-1 == rename("no_matter", FS_TOOLONGNAME));
	test_assert(ENAMETOOLONG == errno);
#ifdef ENABLE_RELATIVE_PATH
	test_assert_zero(rename(FS_FILE1, FS_FILE3_NAME));
	test_assert_zero(rename(FS_FILE3, FS_FILE1_NAME));
#endif
	test_assert_zero(rename(FS_FILE1, FS_FILE3));
	test_assert_zero(rename(FS_FILE3, FS_FILE1));
	/* TODO: add test with recursive renaming */
}


static int setup_suite(void) {

	if (0 != ramdisk_create(FS_DEV, FS_BLOCKS * PAGE_SIZE())) {
		return -1;
	}

	/* format filesystem */
	if(0 != format(FS_DEV, FS_NAME)) {
		return -1;
	}

	/* mount filesystem */
	if(mount(FS_DEV, FS_DIR, FS_NAME)) {
		return -1;
	}

	return creat(FS_FILE1, 0);
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
