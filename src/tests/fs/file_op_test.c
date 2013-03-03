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

/* For command testing */
#include <cmd/cmdline.h>
#include <framework/cmd/api.h>

#include <mem/page.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("fs/file test");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME			"vfat"
#define FS_DEV			"/dev/ramdisk"
#define FS_TYPE			12
#define FS_BLOCKS		124
#define MKDIR_PERM		0700
#define FS_DIR			"/tmp"
#define FS_DIR			"/tmp"
#define FS_FILE1		"/tmp/1/2/3/1.txt"
#define FS_FILE2		"/tmp/1/2/3/2.txt"
#define FS_DIR3			"/tmp/1/2/3"
#define FS_DIR2			"/tmp/1/2"
#define FS_DIR1			"/tmp/1"
#define FS_TESTDATA		"qwerty\n"
#define FS_DTR			"/tmp/dtr"
#define FS_MV_SUB		"/tmp/dtr/sub"
#define FS_MV_SUB_F1	"/tmp/dtr/sub/file1"
#define FS_MV_F1		"/tmp/dtr/file1"
#define FS_MV_F2		"/tmp/dtr/sub/file2"
#define FS_MV_F2_NAME	"file2"
#define FS_MV_F3		"/tmp/dtr/sub/file3"
#define FS_MV_F3_NAME	"file3"
#define FS_MV_RENAMED	"/tmp/renamed"
#define FS_TESTDATA		"qwerty\n"
#define FS_MV_LONGNAME	"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
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

TEST_CASE("Rename file") {
	/* Prepare directories and files for tests */
	test_assert_zero(mkdir(FS_DTR, MKDIR_PERM));
	test_assert_zero(mkdir(FS_MV_SUB, MKDIR_PERM));
	test_assert(-1 != creat(FS_MV_F1, S_IRUSR | S_IWUSR));
	test_assert(-1 != creat(FS_MV_F2, S_IRUSR | S_IWUSR));

	/* Check error code for non-existent file */
	test_assert(-1 == rename("no_such_file", FS_MV_F3));
	test_assert(-ENOENT == errno);

	/* Check error code if destination file exists */
	test_assert(-1 == rename(FS_MV_F1, FS_MV_F2));
	test_assert(-EINVAL == errno);

	/* Check error code with too long source file name */
	test_assert(-1 == rename(FS_MV_LONGNAME, "no_matter"));
	test_assert(-ENAMETOOLONG == errno);

	/* Check error code with too long destination file name */
	test_assert(-1 == rename("no_matter", FS_MV_LONGNAME));
	test_assert(-ENAMETOOLONG == errno);

	/* Test with relative paths */
#ifdef ENABLE_RELATIVE_PATH
	test_assert_zero(rename(FS_MV_F2, FS_MV_F3_NAME));
	test_assert_zero(rename(FS_MV_F3, FS_MV_F2_NAME));
#endif

	/* Renaming one file and renaming it back */
	test_assert_zero(rename(FS_MV_F2, FS_MV_F3));
	test_assert_zero(rename(FS_MV_F3, FS_MV_F2));

	/* Add test with recursive renaming */
	test_assert_zero(rename(FS_DTR, FS_MV_RENAMED));
	test_assert_zero(rename(FS_MV_RENAMED, FS_DTR));

	/* Add tests with directory as destination */
	test_assert_zero(rename(FS_MV_F1, FS_MV_SUB));
	test_assert_zero(rename(FS_MV_SUB_F1, FS_DTR));

	/* Test cleanup */
	test_assert_zero(remove(FS_DTR));
}

/* Exec shell command and return it's exit code */
static int exec_shell_cmd(char *cmdline) {
	const struct cmd *cmd;
	int argc = 0, code;
	/* In the worst case cmdline looks like "x x x x x x". */
	char *argv[(80 + 1) / 2];

	/* Test simple move */
	if (0 == (argc = cmdline_tokenize(cmdline, argv))) {
		/* Only spaces were entered */
		return -1;
	}

	if (NULL == (cmd = cmd_lookup(argv[0]))) {
		printf("%s: Command not found\n", argv[0]);
		return -1;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		printf("%s: Command returned with code %d: %s\n",
				cmd_name(cmd), code, strerror(-code));
	}

	return code;
}

TEST_CASE("Move file") {
	/* This should be improved to not use hard-coded paths */
	char *cmd_recursive_err = "mv /tmp/dtr /tmp/tmpdtr";
	char *cmd_force_err = "mv /tmp/dtr/file1 /tmp/dtr/sub/file2";
	char *cmd_multi_err =
				"mv /tmp/dtr/file1 /tmp/dtr/sub/file2 /tmp/file";

	char *cmd_simple = "mv /tmp/dtr/file1 /tmp/dtr/sub/tmpfile";
	char *cmd_simple_back = "mv /tmp/dtr/sub/tmpfile /tmp/dtr/file1";
	char *cmd_recursive = "mv -r /tmp/dtr /tmp/tmpdtr";
	char *cmd_recursive_back =  "mv -r /tmp/tmpdtr /tmp/dtr";
	char *cmd_multi =
			"mv /tmp/dtr/file1 /tmp/dtr/sub/file2 /tmp";
	char *cmd_force = "mv -f /tmp/file1 /tmp/file2";

	/* Prepare directories and files for tests */
	test_assert_zero(mkdir(FS_DTR, MKDIR_PERM));
	test_assert_zero(mkdir(FS_MV_SUB, MKDIR_PERM));
	test_assert(-1 != creat(FS_MV_F1, S_IRUSR | S_IWUSR));
	test_assert(-1 != creat(FS_MV_F2, S_IRUSR | S_IWUSR));

	/**
	 * Error codes tests
	 */

	/* Move directory without recursive key */
	test_assert(-EINVAL == exec_shell_cmd(cmd_recursive_err));

	/* Overwrite without force key */
	test_assert(-EINVAL == exec_shell_cmd(cmd_force_err));

	/* Multi-source with not directory as destination */
	test_assert(-EINVAL == exec_shell_cmd(cmd_multi_err));

	/**
	 * Valid tests
	 */

	/* Simple test */
	test_assert_zero(exec_shell_cmd(cmd_simple));
	test_assert_zero(exec_shell_cmd(cmd_simple_back));

	/* Recursive test */
	test_assert_zero(exec_shell_cmd(cmd_recursive));
	test_assert_zero(exec_shell_cmd(cmd_recursive_back));

	/* Multi-source test */
	test_assert_zero(exec_shell_cmd(cmd_multi));

	/* Force test */
	test_assert_zero(exec_shell_cmd(cmd_force));

	/* Test cleanup */
	test_assert_zero(remove(FS_DTR));
}

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
