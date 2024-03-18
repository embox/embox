/**
 * @file
 * @brief Testing ext2 xattr with /dev/hda attached to http://embox.googlecode.com/files/ext2_xattr_hd.img
 *
 * @author  Anton Kozlov
 * @date    30.01.2013
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/xattr.h>
#include <unistd.h>

#include <embox/test.h>
#include <mem/page.h>

EMBOX_TEST_SUITE("Extended attributes");

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME                "ext2"
#define FS_DEV                 "/dev/hda"
#define FS_DIR                 "/tmp"

#define TEST_CLEAN_FILE_NM     "/tmp/clean_test"
#define TEST_FILE_NM           "/tmp/test"
#define TEST_FILE2_NM          "/tmp/test2"
#define TEST_FILE_ADD_NM       "/tmp/test_add_with_xattr"
#define TEST_FILE_ADD_CLEAN_NM "/tmp/test_add_clean"

static const char *xattr_nm1 = "attr1";
static const char *xattr_vl1 = "value1";
static const char *xattr_vl1n = "new_value1";

static const char *xattr_nm2 = "attr2";
static const char *xattr_vl2 = "value2";

static const char *xattr_nm3 = "Attr3";
static const char *xattr_vl3 = "value3";

static const char *xattr_nm4 = "1ttr4";
static const char *xattr_vl4 = "value4";

#define MAX_ATTR_L 32
#define MAX_ATTR_N 4
static int check_xattr(const char *path, int fd, const char *name,
    const char *value) {
	char buf[MAX_ATTR_L];
	int ret;
	const int vlen = strlen(value) + 1;

	if (path) {
		ret = getxattr(path, name, buf, MAX_ATTR_L);
	}
	else {
		ret = fgetxattr(fd, name, buf, MAX_ATTR_L);
	}

	if (vlen != ret) {
		return -1;
	}

	return strcmp(value, buf);
}

static int check_xattr_list(const char *path, int fd, const char *xattr_nms[],
    const char *xattr_vls[]) {
	const char **nm, **vl;
	int ret, list_len;

	char buf[MAX_ATTR_L * MAX_ATTR_N];
	char *p = buf;

	int xattrs_len;

	if (path) {
		xattrs_len = listxattr(path, buf, MAX_ATTR_L * MAX_ATTR_N);
	}
	else {
		xattrs_len = flistxattr(fd, buf, MAX_ATTR_L * MAX_ATTR_N);
	}

	list_len = 0;
	for (nm = xattr_nms, vl = xattr_vls; *nm != NULL; nm++, vl++) {
		if (0 != (ret = check_xattr(path, fd, *nm, *vl))) {
			return ret;
		}
		list_len++;
	}

	for (int i = 0; i < list_len; i++) {
		int xlen = strlen(xattr_nms[i]) + 1;

		if (0 != (ret = strcmp(p, xattr_nms[i]))) {
			return ret;
		}

		p += xlen;
	}

	return (p == (buf + xattrs_len)) ? 0 : -1;
}

TEST_CASE("xattr should be ok for clean file") {
	const char *xattr_nms[1] = {};
	const char *xattr_vls[1] = {};

	test_assert_zero(
	    check_xattr_list(TEST_CLEAN_FILE_NM, 0, xattr_nms, xattr_vls));
}

TEST_CASE("xattr should be listed and getted") {
	const char *xattr_nms[5] = {xattr_nm4, xattr_nm3, xattr_nm1, xattr_nm2};
	const char *xattr_vls[5] = {xattr_vl4, xattr_vl3, xattr_vl1, xattr_vl2};

	test_assert_zero(check_xattr_list(TEST_FILE_NM, 0, xattr_nms, xattr_vls));
}

TEST_CASE("xattr should be listed and getted through fd interface") {
	const char *xattr_nms[5] = {xattr_nm4, xattr_nm3, xattr_nm1, xattr_nm2};
	const char *xattr_vls[5] = {xattr_vl4, xattr_vl3, xattr_vl1, xattr_vl2};

	int fd = open(TEST_FILE_NM, O_RDONLY);

	test_assert_zero(check_xattr_list(NULL, fd, xattr_nms, xattr_vls));

	close(fd);
}

TEST_CASE("xattr should be removed") {
	const char *xattr_nms[4] = {xattr_nm4, xattr_nm3, xattr_nm2};
	const char *xattr_vls[4] = {xattr_vl4, xattr_vl3, xattr_vl2};

	test_assert_equal(setxattr(TEST_FILE2_NM, "non_existing_attr", NULL, 0,
	                      XATTR_REMOVE),
	    -1);
	test_assert_equal(ENOENT, errno);

	test_assert_zero(setxattr(TEST_FILE_NM, xattr_nm1, NULL, 0, XATTR_REMOVE));
	test_assert_zero(check_xattr_list(TEST_FILE_NM, 0, xattr_nms, xattr_vls));
}

TEST_CASE("xattr should be replaced") {
	const char *xattr_nms_old[5] = {xattr_nm4, xattr_nm3, xattr_nm1, xattr_nm2};
	const char *xattr_vls_old[5] = {xattr_vl4, xattr_vl3, xattr_vl1, xattr_vl2};

	const char *xattr_nms[5] = {xattr_nm4, xattr_nm3, xattr_nm1, xattr_nm2};
	const char *xattr_vls[5] = {xattr_vl4, xattr_vl3, xattr_vl1n, xattr_vl2};

	test_assert_zero(
	    check_xattr_list(TEST_FILE2_NM, 0, xattr_nms_old, xattr_vls_old));
	test_assert_equal(setxattr(TEST_FILE2_NM, "non_existing_attr", xattr_vl1n,
	                      strlen(xattr_vl1n), XATTR_REPLACE),
	    -1);
	test_assert_equal(ENOENT, errno);

	test_assert_zero(setxattr(TEST_FILE2_NM, xattr_nm1, xattr_vl1n,
	    strlen(xattr_vl1n), XATTR_REPLACE));
	test_assert_zero(check_xattr_list(TEST_FILE2_NM, 0, xattr_nms, xattr_vls));
}

TEST_CASE("xattr entry should be added for file with xattr") {
	const char *xattr_nms[5] = {xattr_nm4, xattr_nm3, xattr_nm1, xattr_nm2};
	const char *xattr_vls[5] = {xattr_vl4, xattr_vl3, xattr_vl1, xattr_vl2};

	test_assert_equal(setxattr(TEST_FILE_ADD_NM, xattr_nm1, xattr_vl1,
	                      strlen(xattr_vl1), XATTR_CREATE),
	    -1);
	test_assert_equal(EEXIST, errno);

	test_assert_zero(setxattr(TEST_FILE_ADD_NM, xattr_nm3, xattr_vl3,
	    strlen(xattr_vl3), XATTR_CREATE));
	test_assert_zero(
	    check_xattr_list(TEST_FILE_ADD_NM, 0, xattr_nms, xattr_vls));
}

TEST_CASE("xattr entry should be added for clean file") {
	const char *xattr_nms[2] = {xattr_nm1};
	const char *xattr_vls[2] = {xattr_vl1};

	test_assert_zero(setxattr(TEST_FILE_ADD_CLEAN_NM, xattr_nm1, xattr_vl1,
	    strlen(xattr_vl1), XATTR_CREATE));

	test_assert_zero(
	    check_xattr_list(TEST_FILE_ADD_CLEAN_NM, 0, xattr_nms, xattr_vls));
}

TEST_CASE("xattr should be setted and retrived on socket") {
	const char *xattr_nms[5] = {xattr_nm4, xattr_nm3, xattr_nm1, xattr_nm2};
	const char *xattr_vls[5] = {xattr_vl4, xattr_vl3, xattr_vl1, xattr_vl2};
	const char **i_xattr, **i_value;
	int sock_fd;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	test_assert(0 <= sock_fd);

	for (i_xattr = xattr_nms, i_value = xattr_vls; *i_xattr && *i_value;
	     i_xattr++, i_value++) {
		test_assert_zero(
		    fsetxattr(sock_fd, *i_xattr, *i_value, strlen(*i_value) + 1, 0));
	}

	test_assert_zero(check_xattr_list(NULL, sock_fd, xattr_nms, xattr_vls));

	close(sock_fd);
}

static int setup_suite(void) {
	int res;

	if ((res = mount(FS_DEV, FS_DIR, FS_NAME, 0, NULL))) {
		return res;
	}

	return 0;
}

static int teardown_suite(void) {
	return 0;
}
