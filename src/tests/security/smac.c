/**
 * @file
 * @brief test for access of http://embox.googlecode.com/files/ext2_smac.img
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <security/smac.h>
#include <fs/vfs.h>
#include <fs/sys/fsop.h>
#include <kernel/task.h>

#include <embox/test.h>

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

const static char *high_static = HIGH;
const static char *low_static = LOW;

#define SMAC_BACKUP_LEN 64

static struct smac_entry smac_backup[SMAC_BACKUP_LEN];
static int smac_back_n;

static struct smac_entry test_env[] = {
	{HIGH, HIGH, S_IRWXO},
	{HIGH, LOW,  S_IROTH},
	{LOW,  HIGH, S_IWOTH},
	{LOW,  LOW,  S_IRWXO},
};

static mode_t root_backup_mode;

static int setup_suite(void) {
	int res;

	if (0 != (res = smac_setenv(test_env, sizeof(test_env) / sizeof(struct smac_entry),
			smac_backup, sizeof(smac_backup), &smac_back_n))) {
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

	vfs_get_root()->mode = root_backup_mode;

	return smac_setenv(smac_backup, smac_back_n, NULL, 0, NULL);
}

static int clear_id(void) {
	struct smac_task *smac_task = (struct smac_task *) task_self_security();

	strcpy(smac_task->label, "_");

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
