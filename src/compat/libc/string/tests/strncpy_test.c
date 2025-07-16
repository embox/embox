/**
 * @file
 *
 * @date Mar 22, 2020
 * @author: Calen Robinette
 */

#include <string.h>
#include <stdlib.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suite for strncpy()");

TEST_CASE("duplicate full string with strncpy()") {
    char src[] = "string";
    char dst[sizeof(src)];

    strncpy(dst, src, sizeof(dst));
    test_assert_not_null(dst);
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("duplicate empty string with strncpy()") {
    char src[] = "";
    char dst[sizeof(src)];

    strncpy(dst, src, sizeof(dst));
    test_assert_not_null(dst);
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("duplicate part of a string with strncpy()") {
    char src[] = "string";
    const int prefix_len = 3;
    char dst[sizeof(src)];

    memset(dst, '\0', sizeof(dst));

    strncpy(dst, src, prefix_len);
    test_assert_not_null(dst);
    test_assert_zero(strncmp(dst, src, prefix_len));
    test_assert_equal(dst[prefix_len], '\0');
}

TEST_CASE("check for NULL argument with strncpy()") {
    char src[] = "\0";
    char dst[sizeof(src)];

    strncpy(dst, src, sizeof(dst));
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("check for zero argument length with strncpy()") {
    char src[] = "string";
    const int copy_len = 0;
    char dst[sizeof(src)];

    memset(dst, '\0', sizeof(dst));

    strncpy(dst, src, copy_len);
    test_assert_not_null(dst);
    test_assert_zero(strncmp(dst, src, copy_len));
    test_assert_equal(dst[0], '\0');
}
