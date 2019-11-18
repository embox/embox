/**
 * @file
 * @brief test for mount to nfs.
 *
 * @date 14.11.19
 * @author Filipp Chubukov
 */

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <embox/test.h>

#include <fs/mount.h>

#define FS_TEST_SHARE_PATH "10.0.2.10:/var/nfs_test"
#define FS_TEST_MOUNTPOINT "/mnt/fs_test"
#define FS_TEST_FS_TYPE "nfs"

static const char fs_test_create_file[] = FS_TEST_MOUNTPOINT "/test_file";
static const char fs_test_create_dir[] = FS_TEST_MOUNTPOINT "/test_dir";

EMBOX_TEST_SUITE("mount nfs tests");

TEST_CASE("Mount, create files and umount") {
	 int fd;

	 test_assert(0 <= mount(FS_TEST_SHARE_PATH, FS_TEST_MOUNTPOINT, FS_TEST_FS_TYPE));

	 test_assert(0 <= (fd = creat(fs_test_create_file, 0666)));

	 close(fd);

	 test_assert(0 <= mkdir(fs_test_create_dir, 0777));

	 test_assert(0 <= umount(FS_TEST_MOUNTPOINT));
}

TEST_CASE("Mount again, check file's info, remove and umount") {
	struct stat st;

	test_assert(0 <= mount(FS_TEST_SHARE_PATH, FS_TEST_MOUNTPOINT, FS_TEST_FS_TYPE));

	stat(fs_test_create_file, &st);

	test_assert_true(S_ISREG(st.st_mode));

	stat(fs_test_create_dir, &st);

	test_assert_true(S_ISDIR(st.st_mode));

	test_assert(0 <= remove(fs_test_create_file));

	test_assert(0 <= remove(fs_test_create_dir));

	test_assert(0 <= umount(FS_TEST_MOUNTPOINT));
}
