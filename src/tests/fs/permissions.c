/**
 * @file
 * @brief Test with http://embox.googlecode.com/files/ext2_perm.img
 *
 * @author  Anton Kozlov
 * @date    12.02.2013
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <fs/sys/fsop.h>/* now mount declaration in this header */

#include <embox/test.h>

EMBOX_TEST_SUITE("Permision tests");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN(clear_id);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME  "ext2"
#define FS_DEV  "/dev/hda"
#define FS_DIR  "/test_permision"

#define TEST_FNM1 "/test_permision/f1"
#define TEST_UID1 1000
#define TEST_GID1 1000
#define TEST_MOD1 0664

#define TEST_FNM2 "/test_permision/f2"
#define TEST_UID2 1001
#define TEST_GID2 1001
#define TEST_MOD2 0006

#define TEST_UID_INVAL 0xdead
#define TEST_GID_INVAL 0xdead

static int setup_suite(void) {
        int res;

        if ((res = mount(FS_DEV, FS_DIR, FS_NAME))) {
                return res;
        }

        return 0;

}

static int teardown_suite(void) {
        return 0;
}

static char temp_buf[4];

#include <kernel/task/u_area.h>
static int clear_id(void) {
	memset(task_self_u_area(), 0, sizeof(struct task_u_area));
	return 0;
}

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
}

TEST_CASE("Own permisions must shadow others permissions") {
	int fd;
	test_assert_zero(setuid(TEST_UID2));
	test_assert_zero(setgid(TEST_GID2));

	test_assert_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_WRONLY));
}

TEST_CASE("Access should be granted for others permissions") {
	int fd;
	test_assert_zero(setuid(TEST_UID1));
	test_assert_zero(setgid(TEST_GID1));

	test_assert_not_equal(-1, fd = open(TEST_FNM1, O_TRUNC | O_RDONLY));
	test_assert_not_equal(-1, read(fd, temp_buf, 0));
	close(fd);
}

