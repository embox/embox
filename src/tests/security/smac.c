/**
 * @file
 * @brief test for access of http://embox.googlecode.com/files/ext2_users.img
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <unistd.h>

#include <embox/test.h>
#include <fs/vfs.h>
#include <kernel/task.h>
#include <kernel/task/resource/security.h>
#include <security/smac/smac.h>

EMBOX_TEST_SUITE("smac tests with classic modes allows all access");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN(clear_id);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME "ext2"
#define FS_DEV  "/dev/hda"
#define FS_DIR  "/tmp"

#define FILE_H  "/tmp/file_high"
#define FILE_L  "/tmp/file_low"

#define HIGH    "high_label"
#define LOW     "low_label"

static const char *high_static = HIGH;
static const char *low_static = LOW;
static const char *smac_star = "*";

#define SMAC_BACKUP_LEN 1024

static char buf[SMAC_BACKUP_LEN];
static struct smac_env *backup;

static struct smac_env test_env = {
    .n = 2,
    .entries =
        {
            {HIGH, LOW, S_IROTH},
            {LOW, HIGH, S_IWOTH},
        },
};

static mode_t root_backup_mode;

static int setup_suite(void) {
	int res;

	if (0 != (res = smac_getenv(buf, SMAC_BACKUP_LEN, &backup))) {
		return res;
	}

	if (0 != (res = smac_setenv(&test_env))) {
		return res;
	}

	root_backup_mode = vfs_get_root()->i_mode;
	vfs_get_root()->i_mode = S_IFDIR | 0777;

	if (-1 == mount(FS_DEV, FS_DIR, FS_NAME)) {
		return -errno;
	}

	return 0;
}

static int teardown_suite(void) {
	//	int res;

	vfs_get_root()->i_mode = root_backup_mode;

	return smac_setenv(backup);
}

static int clear_id(void) {
	struct smac_task *smac_task =
	    (struct smac_task *)task_self_resource_security();

	strcpy(smac_task->label, "smac_admin");

	return 0;
}

TEST_CASE("Low subject shouldn't read high object") {
	int fd;

	smac_labelset(low_static);

	test_assert_equal(-1, fd = open(FILE_H, O_RDWR));
	test_assert_equal(EACCES, errno);

	test_assert_equal(-1, fd = open(FILE_H, O_RDONLY));
	test_assert_equal(EACCES, errno);
}

TEST_CASE("High subject shouldn't write low object") {
	int fd;

	smac_labelset(high_static);

	test_assert_equal(-1, fd = open(FILE_L, O_RDWR));
	test_assert_equal(EACCES, errno);

	test_assert_equal(-1, fd = open(FILE_L, O_WRONLY));
	test_assert_equal(EACCES, errno);
}

TEST_CASE("High subject should be able r/w high object") {
	int fd;

	smac_labelset(high_static);

	test_assert_not_equal(-1, fd = open(FILE_H, O_RDWR));

	close(fd);
}

TEST_CASE("Low subject should be able r/w low object") {
	int fd;

	smac_labelset(low_static);

	test_assert_not_equal(-1, fd = open(FILE_L, O_RDWR));

	close(fd);
}

TEST_CASE("High subject shouldn't be able change high object label") {
	smac_labelset(high_static);

	test_assert_equal(-1,
	    setxattr(FILE_H, smac_xattrkey, smac_star, strlen(smac_star), 0));

	test_assert_equal(EACCES, errno);
}

TEST_CASE("Low subject shouldn't be able change high object label") {
	smac_labelset(low_static);

	test_assert_equal(-1,
	    setxattr(FILE_H, smac_xattrkey, smac_star, strlen(smac_star), 0));

	test_assert_equal(EACCES, errno);
}

TEST_CASE("smac admin should be able change high object label") {
	smac_labelset(smac_admin);

	test_assert_zero(
	    setxattr(FILE_H, smac_xattrkey, smac_star, strlen(smac_star), 0));

	test_assert_zero(
	    setxattr(FILE_H, smac_xattrkey, high_static, strlen(high_static), 0));
}
