/**
 * @file
 *
 * @date May 23, 2025
 * @author Yulong Wang
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("ldexp() tests");

static int equal(double a, double b, double epsilon) {
    return fabs(a - b) < epsilon;
}

TEST_CASE("Test basic functionality: ldexp(0.5, 1)") {
    double x = 0.5;
    int exp = 1;
    double result = ldexp(x, exp);
    
    test_assert(equal(result, 1.0, DBL_EPSILON));
}

TEST_CASE("Test negative mantissa: ldexp(-0.5, 2)") {
    double x = -0.5;
    int exp = 2;
    double result = ldexp(x, exp);
    
    test_assert(equal(result, -2.0, DBL_EPSILON));
}

TEST_CASE("Test zero mantissa: ldexp(0.0, 10)") {
    double x = 0.0;
    int exp = 10;
    double result = ldexp(x, exp);
    
    test_assert(result == 0.0);
}

TEST_CASE("Test negative zero: ldexp(-0.0, 5)") {
    double x = -0.0;
    int exp = 5;
    double result = ldexp(x, exp);
    
    test_assert(result == -0.0);
}

TEST_CASE("Test large exponent: ldexp(0.5, 1024)") {
    double x = 0.5;
    int exp = 1025;
    double result = ldexp(x, exp);
    
    test_assert(isinf(result));
}

TEST_CASE("Test small exponent: ldexp(0.5, -1074)") {
    double x = 0.5;
    int exp = -1074;
    double result = ldexp(x, exp);
    
    test_assert(result == 0.0);
}

TEST_CASE("Test with frexp roundtrip") {
    double original = 123.456;
    int exp;
    double mantissa = frexp(original, &exp);
    double reconstructed = ldexp(mantissa, exp);
    
    test_assert(equal(original, reconstructed, DBL_EPSILON * 10));
}

TEST_CASE("Test subnormal result") {
    double x = 0.5;
    int exp = -1022;
    double result = ldexp(x, exp);
    
    test_assert(result > 0.0 && result < DBL_MIN);
}

TEST_CASE("Test INFINITY mantissa") {
    double x = INFINITY;
    int exp = 10;
    double result = ldexp(x, exp);
    
    test_assert(isinf(result) && result > 0);
}

TEST_CASE("Test -INFINITY mantissa") {
    double x = -INFINITY;
    int exp = 5;
    double result = ldexp(x, exp);
    
    test_assert(isinf(result) && result < 0);
}

TEST_CASE("Test NaN mantissa") {
    double x = NAN;
    int exp = 3;
    double result = ldexp(x, exp);
    
    test_assert(isnan(result));
}