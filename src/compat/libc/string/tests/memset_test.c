/**
 * @file
 *
 * @date Sep 30, 2023
 * @author Semyon Tagiltsev
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suite for memset()");

TEST_CASE("duplicate full string with memset()") {
    char src[] = "***";
    char dst[] = "---";

    memset(dst, '*', sizeof(src));

    test_assert_mem_not_equal(dst, src, sizeof(src));
}

TEST_CASE("check for not equal strings with memset()") {
    char src[] = "string";
    char dst[] = "string";

    memset(dst, '*', sizeof(src));

    test_assert_mem_not_equal(dst, src, sizeof(src));
}

TEST_CASE("check for NULL argument with memset()") {
    char src[] = "\0";
    char dst[sizeof(src) + 1];

    memset(dst, '\0', sizeof(src) + 1);

    test_assert_mem_equal(dst, src, 1);
}

TEST_CASE("check for equal only first N nums with memset()") {
    char src[] = {1, 1, 1, 1, 1};
    char dst[] = {0, 0, 0, 0, 0};

    memset(dst, 1, 3);

    test_assert_mem_equal(dst, src, 3);
    test_assert_mem_not_equal(dst + 3, src + 3, 2);
}

TEST_CASE("check for not equal only first N nums with memset()") {
    char src[] = {1, 1, 1, 1, 1};
    char dst[] = {1, 1, 1, 1, 1};

    memset(dst, 0, 3);

    test_assert_mem_not_equal(dst, src, 3);
    test_assert_mem_equal(dst + 3, src + 3, 2);
}

TEST_CASE("check for equal only last N nums with memset()") {
    char src[] = {1, 1, 1, 1, 1};
    char dst[] = {0, 0, 0, 0, 0};

    memset((dst + sizeof(src)) - 3, 1, 3);

    test_assert_mem_not_equal(dst, src, 2);
    test_assert_mem_equal(dst + 2, src + 2, 3);
}

TEST_CASE("check for not equal only last N nums with memset()") {
    char src[] = {1, 1, 1, 1, 1};
    char dst[] = {1, 1, 1, 1, 1};

    memset((dst + sizeof(src)) - 3, 0, 3);

    test_assert_mem_equal(dst, src, 2);
    test_assert_mem_not_equal(dst + 2, src + 2, 3);
}

TEST_CASE("check for equal only N's number with memset()") {
    char src[] = {1, 1, 1, 1, 1};
    char dst[] = {0, 0, 0, 0, 0};

    memset(dst + 2, 1, 1);

    test_assert_mem_not_equal(dst, src, 2);
    test_assert_mem_equal(dst + 2, src + 2, 1);
    test_assert_mem_not_equal(dst + 3, src + 3, 2);
}

TEST_CASE("check for not equal only N's number with memset()") {
    char src[] = {1, 1, 1, 1, 1};
    char dst[] = {1, 1, 1, 1, 1};

    memset(dst + 2, 0, 1);

    test_assert_mem_equal(dst, src, 2);
    test_assert_mem_not_equal(dst + 2, src + 2, 1);
    test_assert_mem_equal(dst + 3, src + 3, 2);
}
