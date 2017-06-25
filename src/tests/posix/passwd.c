/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <pwd.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("passwd");

#define PASSWD_FILE "/passwd"

#define BUF_LEN 512
#define SMALL_BUF_LEN 1
#define NONEXISTING_NAME "nonexisting"

const struct passwd root_passwd = {
	.pw_name = "root",
	.pw_passwd = "x",
	.pw_uid = 0,
	.pw_gid = 0,
	.pw_gecos = "root",
	.pw_dir = "/root",
	.pw_shell = "/bin/sh",
};

#define PASSWD_ENTRY_CNT 7

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

	test_assert_zero(getpwnam_r(root_passwd.pw_name, &pwd, buf, BUF_LEN, &result));

	test_assert_zero(passwdcmp(&root_passwd, result));
}

TEST_CASE("Existing entry should be find by uid") {
	char buf[BUF_LEN];
	struct passwd pwd, *result;

	test_assert_zero(getpwuid_r(root_passwd.pw_uid, &pwd, buf, BUF_LEN, &result));

	test_assert_zero(passwdcmp(&root_passwd, result));
}

TEST_CASE("Nonexisting entry shouldn't be find") {
	char buf[BUF_LEN];
	struct passwd pwd, *result;

	test_assert_zero(getpwnam_r(NONEXISTING_NAME, &pwd, buf, BUF_LEN, &result));
	test_assert_zero(result);
}

TEST_CASE("fgetpwent should return ERANGE on small buffer") {
	char buf[SMALL_BUF_LEN];
	struct passwd pwd, *result;
	FILE *f = fopen(PASSWD_FILE, "r");

	test_assert_equal(fgetpwent_r(f, &pwd, buf, SMALL_BUF_LEN, &result), ERANGE);

	fclose(f);
}

TEST_CASE("fgetpwent should return ENOENT after last entry") {
	char buf[BUF_LEN];
	struct passwd pwd, *result;
	int i = 0;
	FILE *f = fopen(PASSWD_FILE, "r");

	test_assert_zero(fgetpwent_r(f, &pwd, buf, BUF_LEN, &result)); i++;
	test_assert_zero(passwdcmp(&root_passwd, result));
	test_assert_zero(fgetpwent_r(f, &pwd, buf, BUF_LEN, &result)); i++;
	while (i++ < PASSWD_ENTRY_CNT) {
		test_assert_zero(fgetpwent_r(f, &pwd, buf, BUF_LEN, &result));
	}
	test_assert_equal(fgetpwent_r(f, &pwd, buf, BUF_LEN, &result), ENOENT);

	fclose(f);
}

TEST_CASE("getpwnam should return passwd same as getpwnam_r") {
	char buf[BUF_LEN];
	struct passwd pwd, *result;

	getpwnam_r(root_passwd.pw_name, &pwd, buf, BUF_LEN, &result);

	test_assert_zero(passwdcmp(getpwnam(root_passwd.pw_name), result));
}

