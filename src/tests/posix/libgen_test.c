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


TEST_CASE("basename with 'path' as a NULL pointer") {
	char *bname;

	bname = basename(NULL);

	test_assert_zero(strcmp(bname, "."));
}

TEST_CASE("basename with 'path' points to an empty string") {
	char *bname;

	bname = basename("");

	test_assert_zero(strcmp(bname, "."));
}

TEST_CASE("basename with 'path' equals '/usr/lib'") {
	char *bname;

	bname = basename("/usr/lib");

	test_assert_zero(strcmp(bname, "lib"));
}

TEST_CASE("basename with 'path' equals '/usr/'") {
	char *bname;

	bname = basename("/usr/");

	test_assert_zero(strcmp(bname, "usr"));
}


TEST_CASE("basename with 'path' equals '/'") {
	char *bname;

	bname = basename("/");

	test_assert_zero(strcmp(bname, "/"));
}


TEST_CASE("basename with 'path' equals '///'") {
	char *bname;

	bname = basename("///");

	test_assert_zero(strcmp(bname, "/"));
}


TEST_CASE("basename with 'path' equals '//usr//lib//'") {
	char *bname;

	bname = basename("//usr//lib//");

	test_assert_zero(strcmp(bname, "lib"));
}


/* dirname */
TEST_CASE("dirname with 'path' as a NULL pointer") {
	char *dname;

	dname = dirname(NULL);

	test_assert_zero(strcmp(dname, "."));
}

TEST_CASE("dirname with 'path' points to an empty string") {
	char *dname;

	dname = dirname("");

	test_assert_zero(strcmp(dname, "."));
}

TEST_CASE("dirname with 'path' equals '/usr/lib'") {
	char *dname;

	dname = dirname("/usr/lib");

	test_assert_zero(strcmp(dname, "/usr"));
}

TEST_CASE("dirname with 'path' equals '/usr/'") {
	char *dname;

	dname = dirname("/usr/");

	test_assert_zero(strcmp(dname, "/"));
}


TEST_CASE("dirname with 'path' equals 'usr'") {
	char *dname;

	dname = dirname("usr");

	test_assert_zero(strcmp(dname, "."));
}

TEST_CASE("dirname with 'path' equals '/'") {
	char *dname;

	dname = dirname("/");

	test_assert_zero(strcmp(dname, "/"));
}


TEST_CASE("dirname with 'path' equals '.'") {
	char *dname;

	dname = dirname(".");

	test_assert_zero(strcmp(dname, "."));
}


TEST_CASE("dirname with 'path' equals '..'") {
	char *dname;

	dname = dirname("..");

	test_assert_zero(strcmp(dname, "."));
}
