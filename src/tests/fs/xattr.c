/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.01.2013
 */

#include <unistd.h>
#include <fcntl.h>

#include <fs/sys/fsop.h>/* now mount declaration in this header */
#include <drivers/ramdisk.h>
#include <mem/page.h>
#include <fs/xattr.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Extended attributes");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME  "ext2"
#define FS_DEV  "/dev/hda"
#define FS_DIR  "/test_xattr"

#define TEST_FILE_NM "/test_xattr/test"

static const char *xattr_nm = "attr1";
static const char *xattr_vl = "value1";

TEST_CASE("xattr should be ok for clean file") {
        test_assert_zero(listxattr(TEST_FILE_NM, NULL, 0));
}

#define BUF_LEN 32
TEST_CASE("xattr should be added and then removed") {
        char buf[BUF_LEN];
        test_assert_zero(setxattr(TEST_FILE_NM, xattr_nm, xattr_vl, strlen(xattr_vl),
                                XATTR_CREATE));

        test_assert_equal(listxattr(TEST_FILE_NM, NULL, 0), strlen(xattr_nm) + 1);
        test_assert_equal(getxattr(TEST_FILE_NM, xattr_nm, buf, BUF_LEN), strlen(xattr_vl) + 1);

        test_assert_zero(setxattr(TEST_FILE_NM, xattr_nm, NULL, 0, XATTR_REMOVE));
        test_assert_zero(listxattr(TEST_FILE_NM, NULL, 0));
}


static int setup_suite(void) {
        int res;

        if ((res = mount(FS_DEV, FS_DIR, FS_NAME))) {
                return res;
        }

        unlink(TEST_FILE_NM);

        if ((res = creat(TEST_FILE_NM, 0))) {
                return res;
        }

        return 0;

}

static int teardown_suite(void) {
	return unlink(TEST_FILE_NM);
}
