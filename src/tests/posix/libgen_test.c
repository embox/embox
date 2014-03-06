/**
 * @file
 *
 * @details http://pubs.opengroup.org/onlinepubs/009604599/functions/dirname.html
 *
 * @date Oct 28, 2013
 * @author: Anton Bondarev
 */
#include <libgen.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("testing functions from libgen.h");

static char *strcopy(const char *str) {
	static char str_buf[256];
	return strcpy(str_buf, str);
}

TEST_CASE("basename with 'path' as a NULL pointer") {
	char *bname;

	bname = basename(NULL);

	test_assert_zero(strcmp(bname, "."));
}

TEST_CASE("basename with 'path' points to an empty string") {
	char *bname;

	bname = basename(strcopy(""));

	test_assert_zero(strcmp(bname, "."));
}

TEST_CASE("basename with 'path' equals '/usr/lib'") {
	char *bname;

	bname = basename(strcopy("/usr/lib"));

	test_assert_zero(strcmp(bname, "lib"));
}

TEST_CASE("basename with 'path' equals '/usr/'") {
	char *bname;

	bname = basename(strcopy("/usr/"));

	test_assert_zero(strcmp(bname, "usr"));
}


TEST_CASE("basename with 'path' equals '/'") {
	char *bname;

	bname = basename(strcopy("/"));

	test_assert_zero(strcmp(bname, "/"));
}


TEST_CASE("basename with 'path' equals '///'") {
	char *bname;

	bname = basename(strcopy("///"));

	test_assert_zero(strcmp(bname, "/"));
}


TEST_CASE("basename with 'path' equals '//usr//lib//'") {
	char *bname;

	bname = basename(strcopy("//usr//lib//"));

	test_assert_zero(strcmp(bname, "lib"));
}

TEST_CASE("basename with 'path' equals 'filename'") {
	char *bname;

	bname = basename(strcopy("filename"));

	test_assert_zero(strcmp(bname, "filename"));
}

/* dirname */
TEST_CASE("dirname with 'path' as a NULL pointer") {
	char *dname;

	dname = dirname(NULL);

	test_assert_zero(strcmp(dname, "."));
}

TEST_CASE("dirname with 'path' points to an empty string") {
	char *dname;

	dname = dirname(strcopy(""));

	test_assert_zero(strcmp(dname, "."));
}

TEST_CASE("dirname with 'path' equals '/usr/lib'") {
	char *dname;

	dname = dirname(strcopy("/usr/lib"));

	test_assert_zero(strcmp(dname, "/usr"));
}

TEST_CASE("dirname with 'path' equals '/usr/'") {
	char *dname;

	dname = dirname(strcopy("/usr/"));

	test_assert_zero(strcmp(dname, "/"));
}


TEST_CASE("dirname with 'path' equals 'usr'") {
	char *dname;

	dname = dirname(strcopy("usr"));

	test_assert_zero(strcmp(dname, "."));
}

TEST_CASE("dirname with 'path' equals '/'") {
	char *dname;

	dname = dirname(strcopy("/"));

	test_assert_zero(strcmp(dname, "/"));
}


TEST_CASE("dirname with 'path' equals '.'") {
	char *dname;

	dname = dirname(strcopy("."));

	test_assert_zero(strcmp(dname, "."));
}


TEST_CASE("dirname with 'path' equals '..'") {
	char *dname;

	dname = dirname(strcopy(".."));

	test_assert_zero(strcmp(dname, "."));
}
