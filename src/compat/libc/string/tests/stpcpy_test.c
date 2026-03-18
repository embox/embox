/**
 * @file
 * @brief
 *
 * @author  Vedant Malkar
 * @date    17.03.2026
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("stpcpy testing suite");

TEST_CASE("copy a string with stpcpy"){
    char src[] = "string";
    char dst[sizeof(src)];

    stpcpy(dst,src);
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("copy an empty string with stpcpy()") {
    char dst[9];
    char src[] = "";

    char *end = stpcpy(dst, src);
    test_assert_zero(strcmp(dst, src));
    test_assert_equal(end, dst);
}

TEST_CASE("return value points to the null terminator"){
    char src[] = "hello";
    char dst[sizeof(src)];

    char *end = stpcpy(dst,src);
    test_assert_equal(end, dst + strlen(src));
    test_assert_equal(*end, '\0');
}

TEST_CASE("chain two stpcpy() calls") {
    char dst[16];

    char *end = stpcpy(dst, "hello");
    stpcpy(end, " world");
    test_assert_zero(strcmp(dst, "hello world"));
}