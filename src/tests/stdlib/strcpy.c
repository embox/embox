/**
 * @file
 *
 * @date Mar 22, 2020
 * @author: Calen Robinette
 */

#include <string.h>
#include <stdlib.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suite for strcpy()");

TEST_CASE("duplicate full string with strcpy()") {
    char src[] = "string";
    char dst[sizeof(src) + 1];

    strcpy(dst, src);
    test_assert_not_null(dst);
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("duplicate empty string with strcpy()") {
    char src[] = "";
    char dst[sizeof(src) + 1];

    strcpy(dst, src);
    test_assert_not_null(dst);
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("check for NULL argument with strcpy()") {
    char src[] = "\0";
    char dst[sizeof(src)];

    strcpy(dst, src);
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("check for error if dst isn't allocated enough memory for strcpy()") {
    char src[] = "string";
    char dst[3];

    strcpy(dst, src);
    test_assert_not_zero(strcmp(dst, src));
}
