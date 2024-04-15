/**
 * @file
 * @brief test for mount to nfs.
 *
 * @date 14.11.19
 * @author Filipp Chubukov
 */

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mount.h>
#include <unistd.h>

#include <embox/test.h>
#include <framework/mod/options.h>

EMBOX_TEST_SUITE("mount test");

#define FS_TEST_SHARE_PATH OPTION_STRING_GET(share_path)
#define FS_TEST_FS_TYPE    OPTION_STRING_GET(fs_type)
#define FS_TEST_MOUNTPOINT "/mnt/fs_test"

static const char fs_test_create_file[] = FS_TEST_MOUNTPOINT "/test_file";
static const char fs_test_create_dir[] = FS_TEST_MOUNTPOINT "/test_dir";

TEST_CASE("Mount, create files, umount, mount again and check correct file "
          "stats") {
	int fd;
	struct stat st;

	test_assert(0 <= mount(FS_TEST_SHARE_PATH, FS_TEST_MOUNTPOINT,
	                FS_TEST_FS_TYPE, 0, NULL));

	test_assert(0 <= (fd = creat(fs_test_create_file, 0666)));

	close(fd);

	test_assert(0 <= mkdir(fs_test_create_dir, 0777));

	test_assert(0 <= umount(FS_TEST_MOUNTPOINT));

	test_assert(0 <= mount(FS_TEST_SHARE_PATH, FS_TEST_MOUNTPOINT,
	                FS_TEST_FS_TYPE, 0, NULL));

	stat(fs_test_create_file, &st);

	test_assert_true(S_ISREG(st.st_mode));

	stat(fs_test_create_dir, &st);

	test_assert_true(S_ISDIR(st.st_mode));

	test_assert(0 <= remove(fs_test_create_file));

	test_assert(0 <= remove(fs_test_create_dir));

	test_assert(0 <= umount(FS_TEST_MOUNTPOINT));
}
