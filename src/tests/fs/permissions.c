/**
 * @file
 * @brief Test with http://embox.googlecode.com/files/ext2_users.img
 *
 * @author  Anton Kozlov
 * @date    12.02.2013
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <unistd.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Permision tests");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN(clear_id);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME        "ext2"
#define FS_DEV         "/dev/hda"
#define FS_DIR         "/tmp"

#define TEST_FNM1      "/tmp/f1"
#define TEST_UID1      1000
#define TEST_GID1      1000
#define TEST_MOD1      0664

#define TEST_FNM2      "/tmp/f2"
#define TEST_UID2      1001
#define TEST_GID2      1001
#define TEST_MOD2      0006

#define TEST_DNM1      "/tmp/dir"
#define TEST_DUID1     1000
#define TEST_DGID1     1000
#define TEST_DMOD1     0710

#define TEST_FNM3      "/tmp/dir/f1"
#define TEST_UID3      0
#define TEST_GID3      0
#define TEST_MOD3      0777

#define TEST_FNM4      "/tmp/dir/f2"

#define TEST_DNM2      "/tmp/dir2"

#define TEST_UID_INVAL 0xdead
#define TEST_GID_INVAL 0xdead

#include <fs/vfs.h>
static mode_t root_backup_mode;

static int setup_suite(void) {
	int res;

	/*XXX*/
	root_backup_mode = vfs_get_root()->i_mode;
	vfs_get_root()->i_mode = S_IFDIR | 0777;

	if ((res = mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL))) {
		return res;
	}

	return 0;
}

static int teardown_suite(void) {
	vfs_get_root()->i_mode = root_backup_mode;
	return 0;
}

#include <kernel/task/resource/u_area.h>
static int clear_id(void) {
	memset(task_self_resource_u_area(), 0, sizeof(struct task_u_area));
	return 0;
}

static char temp_buf[4];

TEST_CASE("Writeable file must be writeable for owner") {
	int fd;
	test_assert_zero(setuid(TEST_UID1));
	test_assert_zero(setgid(TEST_GID1));

	test_assert_not_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_WRONLY));
	test_assert_not_equal(-1, write(fd, temp_buf, 0));
	close(fd);
}

TEST_CASE("Writeable file must be writeable for group") {
	int fd;
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_GID1));

	test_assert_not_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_WRONLY));
	test_assert_not_equal(-1, write(fd, temp_buf, 0));
	close(fd);
}

TEST_CASE("Readable file must be readable for others") {
	int fd;
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_GID_INVAL));

	test_assert_not_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_RDONLY));
	test_assert_not_equal(-1, read(fd, temp_buf, 0));
	close(fd);
}

TEST_CASE("Readable file must be not writeable for others") {
	int fd;
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_GID_INVAL));

	test_assert_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_WRONLY));
	test_assert_equal(errno, EACCES);
}

TEST_CASE("Own permisions must shadow others permissions") {
	int fd;
	test_assert_zero(setuid(TEST_UID2));
	test_assert_zero(setgid(TEST_GID2));

	test_assert_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_WRONLY));
	test_assert_equal(errno, EACCES);
}

TEST_CASE("Access should be granted for others permissions") {
	int fd;
	test_assert_zero(setuid(TEST_UID1));
	test_assert_zero(setgid(TEST_GID1));

	test_assert_not_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_RDONLY));
	test_assert_not_equal(-1, read(fd, temp_buf, 0));
	close(fd);
}

TEST_CASE("Opening should be allowed for executable dir") {
	int fd;
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_DGID1));

	test_assert_not_equal(-1, fd = open(TEST_FNM3, O_RDONLY));
	test_assert_not_equal(-1, read(fd, temp_buf, 0));
	close(fd);
}

TEST_CASE("Opening should not be allowed for not executable dir") {
	int fd;
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_GID_INVAL));

	test_assert_equal(-1, fd = open(TEST_FNM3, O_RDONLY));
	test_assert_equal(errno, EACCES);
}

TEST_CASE("Listing should be allowed for readable dir") {
	DIR *d;
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_DGID1));

	test_assert_not_null(d = opendir(TEST_DNM1));
	closedir(d);
}

TEST_CASE("Listing should not be allowed for not readable dir") {
	DIR *d;
	test_assert_zero(setuid(TEST_DGID1));
	test_assert_zero(setgid(TEST_DGID1));

	test_assert_null(d = opendir(TEST_DNM1));
	test_assert_equal(errno, EACCES);
}

TEST_CASE("Creating should be allowed for writeable dir") {
	int fd;
	test_assert_zero(setuid(TEST_UID3));
	test_assert_zero(setgid(TEST_GID3));

	test_assert_not_equal(-1, fd = creat(TEST_FNM4, 0777));
	test_assert_not_equal(-1, write(fd, temp_buf, 0));
	close(fd);

	unlink(TEST_FNM4);
}

TEST_CASE("Creating should not be allowed for not writeable dir") {
	int fd;
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_DGID1));

	test_assert_equal(-1, fd = creat(TEST_FNM4, 0777));
	test_assert_equal(errno, EACCES);
}

TEST_CASE("mkdir should be allowed for writeable dir") {
	test_assert_zero(setuid(TEST_UID3));
	test_assert_zero(setgid(TEST_GID3));

	test_assert_not_equal(-1, mkdir(TEST_DNM2, 0777));

	test_assert_not_equal(-1, rmdir(TEST_DNM2));
}

TEST_CASE("mkdir should not be allowed for not writeable dir") {
	test_assert_zero(setuid(TEST_UID_INVAL));
	test_assert_zero(setgid(TEST_DGID1));

	test_assert_equal(-1, mkdir(TEST_DNM2, 0777));
	test_assert_equal(errno, EACCES);
}
