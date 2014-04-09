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
#include <dirent.h>
#include <embox/test.h>

__EMBOX_TEST_SUITE_AUTORUN("fs read tests", false);

#define FS_TEST_MOUNTPOINT "/mnt/fs_test"

static char fs_test_temp_buf[512];

static const char fs_test_rd_file[] = FS_TEST_MOUNTPOINT "/rd_file";
static const char fs_test_rd_file_content[] = "This is read-only file with original content string\n";
TEST_CASE("Test read operation on fs") {
	int fd;
	int nread;

	test_assert(0 <= (fd = open(fs_test_rd_file, O_RDONLY)));

	nread = read(fd, fs_test_temp_buf, sizeof(fs_test_temp_buf));

	test_assert_equal(nread, sizeof(fs_test_rd_file_content) - 1);
	test_assert_zero(memcmp(fs_test_temp_buf, fs_test_rd_file_content,
				sizeof(fs_test_rd_file_content) - 1));

	close(fd);
}

TEST_CASE("Test stat operations on fs") {
	struct stat st, fst;
	int fd;

	stat(fs_test_rd_file, &st);

	test_assert(0 <= (fd = open(fs_test_rd_file, O_RDONLY)));
	test_assert_zero(fstat(fd, &fst));
	close(fd);

	test_assert(0 == memcmp(&st, &fst, sizeof(struct stat)));

	test_assert_true(S_ISREG(st.st_mode));
	test_assert_equal(st.st_size, sizeof(fs_test_rd_file_content) - 1);
}

static const char fs_test_rd_dir[] = FS_TEST_MOUNTPOINT "/rd_dir";
static const char *fs_test_rd_dir_ents[] = {
	"f1",
	"f2",
	"f3",
};
TEST_CASE("Test readdir operations on fs") {
	struct dirent *dent;
	DIR *d;
	int i;

	d = opendir(fs_test_rd_dir);
	test_assert_not_null(d);

	for (i = 0; i < ARRAY_SIZE(fs_test_rd_dir_ents); i++) {
		dent = readdir(d);

		test_assert_zero(strcmp(dent->d_name, fs_test_rd_dir_ents[i]));
	}

	closedir(d);
}
