/**
 * @file
 * @brief Test unit for stdlib/mbstowcs and stdlib/mbtowc.
 *
 * @date March 26, 2026
 * @author Arhan Chavare
 */

#include <embox/test.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

EMBOX_TEST_SUITE("stdlib/strtod_test");

#define EPS 1e-6
#define DOUBLE_EQ(a, b) (fabs((a) - (b)) < EPS)

TEST_CASE("Simple number") {
    double res = strtod("123.45", NULL);
    test_assert(DOUBLE_EQ(res, 123.45));
}

TEST_CASE("Negative number") {
    double res = strtod("-56.78", NULL);
    test_assert(DOUBLE_EQ(res, -56.78));
}

TEST_CASE("Trailing characters") {
    char *end;
    double res = strtod("12.34abc", &end);
    test_assert(DOUBLE_EQ(res, 12.34));
    test_assert(*end == 'a');
}

TEST_CASE("Invalid input") {
    const char *input = "abc";
    char *end;
    double res = strtod(input, &end);
    test_assert(res == 0.0);
    test_assert(end == input);
}

TEST_CASE("Scientific notation") {
    double res = strtod("1.2e3", NULL);
    test_assert(DOUBLE_EQ(res, 1200.0));
}

TEST_CASE("Positive exponent") {
    double res = strtod("2.5E2", NULL);
    test_assert(DOUBLE_EQ(res, 250.0));
}

TEST_CASE("Negative exponent") {
    double res = strtod("1e-2", NULL);
    test_assert(DOUBLE_EQ(res, 0.01));
}

TEST_CASE("Plus sign") {
    double res = strtod("+99.5", NULL);
    test_assert(DOUBLE_EQ(res, 99.5));
}

TEST_CASE("Empty string") {
    const char *input = "";
    char *end;
    double res = strtod(input, &end);
    test_assert(res == 0.0);
    test_assert(end == input);  
}