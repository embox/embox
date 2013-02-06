/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

#include <types.h>
#include <pwd.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("passwd");

#define TEST_DB "/test_passwd"

#define BUF_LEN (512)

const struct passwd root_passwd = {
	.pw_name = "root",
	.pw_passwd = "x",
	.pw_uid = 0,
	.pw_gid = 0,
	.pw_gecos = "root",
	.pw_dir = "/root",
	.pw_shell = "/bin/false",
};

static int passwdcmp(const struct passwd *p1, const struct passwd *p2) {
	return strcmp(p1->pw_name, p2->pw_name) ||
		strcmp(p1->pw_passwd, p2->pw_passwd) ||
		(p1->pw_uid == p2->pw_uid ? 0 : 1) ||
		(p1->pw_gid == p2->pw_gid ? 0 : 1) ||
		strcmp(p1->pw_gecos, p2->pw_gecos) ||
		strcmp(p1->pw_dir, p2->pw_dir) ||
		strcmp(p1->pw_shell, p2->pw_shell);
}

TEST_CASE("Existing entry should be find by name") {
	char buf[BUF_LEN];
	struct passwd pwd, *result;

	getpwby_db_r(TEST_DB, root_passwd.pw_name, 0, &pwd, buf, BUF_LEN, &result);

	test_assert_zero(passwdcmp(&root_passwd, result));
}

TEST_CASE("Existing entry should be find by uid") {
	char buf[BUF_LEN];
	struct passwd pwd, *result;

	getpwby_db_r(TEST_DB, NULL, root_passwd.pw_uid, &pwd, buf, BUF_LEN, &result);

	test_assert_zero(passwdcmp(&root_passwd, result));
}
