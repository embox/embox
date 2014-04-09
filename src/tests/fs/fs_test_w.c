/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 * @author Anton Kozlov
 */

#include <fcntl.h>
#include <unistd.h>
#include <embox/test.h>

__EMBOX_TEST_SUITE_AUTORUN("fs write tests", false);

#define FS_TEST_MOUNTPOINT "/mnt/fs_test"
#define FS_TEST_CREAT_MODE 0666

static void fs_test_write_file(const char *path, int open_flags, const char *content) {
	int fd;
	int nwritten;
	size_t content_len = strlen(content);

	test_assert(0 <= (fd = open(path, open_flags, FS_TEST_CREAT_MODE)));

	nwritten = write(fd, content, content_len);
	test_assert_equal(nwritten, content_len);

	close(fd);
}

static const char fs_test_wr_file[] = FS_TEST_MOUNTPOINT "/wr_file";
static const char fs_test_wr_file_content[] = "This is content for write file, long and original\n";
TEST_CASE("Test write operation on fs") {

	fs_test_write_file(fs_test_wr_file, O_WRONLY, fs_test_wr_file_content);
}

static const char fs_test_wr_file_new[] = FS_TEST_MOUNTPOINT "/wr_fnew";
static const char fs_test_wr_file_new_content[] = "This is content for new write file, even longer and more original\n";
TEST_CASE("Test create operation on fs") {
	fs_test_write_file(fs_test_wr_file_new, O_WRONLY | O_CREAT | O_EXCL, fs_test_wr_file_new_content);
}

#define FS_TEST_MKDIR_MODE 0777
static const char fs_test_wr_dir[] = FS_TEST_MOUNTPOINT "/wr_dir";
static const char *fs_test_wr_dir_files[] = {
	FS_TEST_MOUNTPOINT "/wr_dir/f1",
	FS_TEST_MOUNTPOINT "/wr_dir/f2",
};
TEST_CASE("Test mkdir operation on fs") {
	int i;

	test_assert_zero(mkdir(fs_test_wr_dir, FS_TEST_MKDIR_MODE));

	for (i = 0; i < ARRAY_SIZE(fs_test_wr_dir_files); i++) {
		fs_test_write_file(fs_test_wr_dir_files[i], O_WRONLY | O_CREAT | O_EXCL, "");
	}
}
