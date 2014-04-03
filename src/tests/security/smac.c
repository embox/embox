/**
 * @file
 * @brief test for access of http://embox.googlecode.com/files/ext2_users.img
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <security/smac.h>
#include <fs/vfs.h>
#include <fs/fsop.h>
#include <fs/flags.h>
#include <kernel/task.h>
#include <sys/xattr.h>

#include <embox/test.h>
#include <kernel/task/resource/security.h>

EMBOX_TEST_SUITE("smac tests with classic modes allows all access");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN(clear_id);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME  "ext2"
#define FS_DEV  "/dev/hda"
#define FS_DIR  "/tmp"

#define FILE_H "/tmp/file_high"
#define FILE_L "/tmp/file_low"

#define HIGH "high_label"
#define LOW  "low_label"

static const char *high_static = HIGH;
static const char *low_static = LOW;
static const char *smac_star = "*";

#define SMAC_BACKUP_LEN 1024

static char buf[SMAC_BACKUP_LEN];
static struct smac_env *backup;

static struct smac_env test_env = {
	.n = 2,
	.entries = {
		{HIGH, LOW,  FS_MAY_READ },
		{LOW,  HIGH, FS_MAY_WRITE},
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

	root_backup_mode = vfs_get_root()->mode;
	vfs_get_root()->mode = S_IFDIR | 0777;

	if ((res = mount(FS_DEV, FS_DIR, FS_NAME))) {
		return res;
	}

	return 0;

}

static int teardown_suite(void) {
	int res;

	vfs_get_root()->mode = root_backup_mode;

	return smac_setenv(backup);
}

static int clear_id(void) {
	struct smac_task *smac_task = (struct smac_task *) task_self_resource_security();

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

	test_assert_equal(-1, setxattr(FILE_H, smac_xattrkey, smac_star,
				strlen(smac_star), 0));

	test_assert_equal(EACCES, errno);

}

TEST_CASE("Low subject shouldn't be able change high object label") {

	smac_labelset(low_static);

	test_assert_equal(-1, setxattr(FILE_H, smac_xattrkey, smac_star,
				strlen(smac_star), 0));

	test_assert_equal(EACCES, errno);

}

TEST_CASE("smac admin should be able change high object label") {

	smac_labelset(smac_admin);

	test_assert_zero(setxattr(FILE_H, smac_xattrkey, smac_star,
				strlen(smac_star), 0));

	test_assert_zero(setxattr(FILE_H, smac_xattrkey, high_static,
				strlen(high_static), 0));
}
