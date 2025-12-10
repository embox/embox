/**
 * @file
 * @brief Test for fnmatch function.
 *
 * @date 19.12.18
 * @author Filipp Chubukov
 */

#include <fnmatch.h>
#include <embox/test.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define TEST_STRING "abc"

EMBOX_TEST_SUITE("posix/fnmatch test");

TEST_CASE("Wrong pattern") {
	char test_string[] = TEST_STRING;
	char pattern[] = "a[v]gg[g[a][";
	
	test_assert(fnmatch(pattern, test_string, 0) == EINVAL);
}

TEST_CASE("String fits a '*' and '?' patterns") {
	char test_string[] = TEST_STRING;
	char pattern[] = "a*?c";
	
	test_assert(fnmatch(pattern, test_string, 0) == 0);
}

TEST_CASE("String not fits a '*' and '?' patterns") {
	char test_string[] = TEST_STRING;
	char pattern[] = "a*?bc";

	test_assert(fnmatch(pattern, test_string, 0) == FNM_NOMATCH);
}
TEST_CASE("String fits a '[]' pattern") {
	char test_string[] = TEST_STRING;
	char pattern[] = "a[abc]c";
	
	test_assert(fnmatch(pattern, test_string, 0) == 0);
}

TEST_CASE("String not fits a '[]' pattern") {
	char test_string[] = TEST_STRING;
	char pattern[] = "a[vfg]c";

	test_assert(fnmatch(pattern, test_string, 0) == FNM_NOMATCH);
}

TEST_CASE("String fits a '[^]' pattern") {
	char test_string[] = TEST_STRING;
	char pattern[] = "a[^ac]c";
	
	test_assert(fnmatch(pattern, test_string, 0) == 0);
}

TEST_CASE("String not fits a '[^]' pattern") {
	char test_string[] = TEST_STRING;
	char pattern[] = "a[^bc]c";

	test_assert(fnmatch(pattern, test_string, 0) == FNM_NOMATCH);
}

TEST_CASE("String fits all patterns") {
	char test_string[] = TEST_STRING;
	char pattern[] = "*[abc]?[^ab]*";
	
	test_assert(fnmatch(pattern, test_string, 0) == 0);
}

TEST_CASE("filepath fits pattern") {
	char test_string[] = "/123/123/123";
	char pattern[] = "/*/?23/123";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == 0);
}

TEST_CASE("filepath not fits pattern with slashes, more slashes in pattern") {
	char test_string[] = "a/b";
	char pattern[] = "a//b";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == FNM_NOMATCH);
}

TEST_CASE("filepath not fits pattern with slashes, more slashes in string") {
	char test_string[] = "a//b";
	char pattern[] = "a/b";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == FNM_NOMATCH);
}

TEST_CASE("filepath not fits pattern with '*' pattern") {
	char test_string[] = "/a";
	char pattern[] = "*a";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == FNM_NOMATCH);
}

TEST_CASE("filepath not fits pattern with first slash") {
	char test_string[] = "123/123/123";
	char pattern[] = "/123/123/123";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == FNM_NOMATCH);
}

TEST_CASE("filepath not fits pattern") {
	char test_string[] = "/123";
	char pattern[] = "/asd";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == FNM_NOMATCH);
}

TEST_CASE("filepath with /[/]") {
	char test_string[] = "/[a/b]";
	char pattern[] = "/[a/b]";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == 0);
}

TEST_CASE("filepath not fits pattern with '*'") {
	char test_string[] = "a/b/c/d";
	char pattern[] = "a/*/d";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == FNM_NOMATCH);
}

TEST_CASE("filepath fits pattern with '/*/'") {
	char test_string[] = "a//b";
	char pattern[] = "a/*/b";
	
	test_assert(fnmatch(pattern, test_string, FNM_PATHNAME) == 0);
}
