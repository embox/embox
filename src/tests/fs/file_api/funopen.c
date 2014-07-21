/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.07.2014
 */

#include <errno.h>
#include <stdio.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suit for funopen()");

static int test_fu_read_cookie(void *cookie, char *buf, int buflen) {
	memset(buf, (unsigned long) cookie, buflen);
	return buflen;
}

static int test_fu_check(const void *cookie, char *buf, int buflen) {
	int i;
	char charcookie = (unsigned long) cookie;

	for (i = 0; i < buflen; i++) {
		if (buf[i] != charcookie) {
			return 1;
		}
	}
	return 0;
}

TEST_CASE("funopen should operate right with only openfn") {
	FILE *file;
	const void *cookie = (void *) 0xcb;
	char buf[128];

	file = funopen(cookie, test_fu_read_cookie, NULL, NULL, NULL);
	test_assert_not_null(file);
	test_assert_equal(sizeof(buf), fread(buf, 1, sizeof(buf), file));
	test_assert_zero(test_fu_check(cookie, buf, sizeof(buf)));

	fclose(file);
}

TEST_CASE("funopen should store cookie in file") {
	FILE *file1, *file2;
	const void *cookie1 = (void *) 0xcb;
	const void *cookie2 = (void *) 0x45;
	char buf[128];

	file1 = funopen(cookie1, test_fu_read_cookie, NULL, NULL, NULL);
	test_assert_not_null(file1);
	file2 = funopen(cookie2, test_fu_read_cookie, NULL, NULL, NULL);
	test_assert_not_null(file1);

	test_assert_equal(sizeof(buf), fread(buf, 1, sizeof(buf), file1));
	test_assert_zero(test_fu_check(cookie1, buf, sizeof(buf)));

	test_assert_equal(sizeof(buf), fread(buf, 1, sizeof(buf), file2));
	test_assert_zero(test_fu_check(cookie2, buf, sizeof(buf)));

	fclose(file1);
	fclose(file2);
}
