/**
 * @file
 *
 * @date May 9, 2025
 * @author Zhiwei Zhang
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <float.h> // For DBL_EPSILON

#include <embox/test.h>

EMBOX_TEST_SUITE("frexp() tests");

static int equal(double a, double b, double epsilon) {
    return fabs(a - b) < epsilon;
}

static int validate(double x, double mantissa, int exponent) {
    if (x == 0.0 || isinf(x) || isnan(x)){ 
        return 1;
    }
    return equal(x, mantissa * ldexp(1.0, exponent), DBL_EPSILON * 10);
}

TEST_CASE("Test positive number: frexp(12.34)") {
    int exp;
    double x = 12.34;
    double mantissa = frexp(x, &exp);

    test_assert(mantissa >= 0.5 && mantissa < 1.0);
    test_assert(validate(x, mantissa, exp));
}

TEST_CASE("Test negative number: frexp(-12.34)") {
    int exp;
    double x = -12.34;
    double mantissa = frexp(x, &exp);

    test_assert(mantissa > -1.0 && mantissa <= -0.5);
    test_assert(validate(x, mantissa, exp));
}

TEST_CASE("Test zero: frexp(0.0)") {
    int exp;
    double x = 0.0;
    double mantissa = frexp(x, &exp);

    test_assert(mantissa == 0.0);
    test_assert(exp == 0);
}

TEST_CASE("Test negative zero: frexp(-0.0)") {
    int exp;
    double x = -0.0;
    double mantissa = frexp(x, &exp);

    test_assert(mantissa == -0.0);
    test_assert(exp == 0);
}

TEST_CASE("Test +INFINITY") {
    int exp;
    double x = INFINITY;
    double mantissa = frexp(x, &exp);

    test_assert(isinf(mantissa) && mantissa > 0);
}

TEST_CASE("Test -INFINITY") {
    int exp;
    double x = -INFINITY;
    double mantissa = frexp(x, &exp);

    test_assert(isinf(mantissa) && mantissa < 0);
}

TEST_CASE("Test NaN") {
    int exp;
    double x = NAN;
    double mantissa = frexp(x, &exp);

    test_assert(isnan(mantissa));
}

TEST_CASE("Test exact power of two: frexp(1024.0)") {
    int exp;
    double x = 1024.0;
    double mantissa = frexp(x, &exp);

    test_assert(equal(mantissa, 0.5, DBL_EPSILON));
    test_assert(exp == 11);
    test_assert(validate(x, mantissa, exp));
}

TEST_CASE("Test subnormal number: frexp(0.375)") {
    int exp;
    double x = 0.375;
    double mantissa = frexp(x, &exp);

    test_assert(equal(mantissa, 0.75, DBL_EPSILON));
    test_assert(exp == -1);
    test_assert(validate(x, mantissa, exp));
}

TEST_CASE("Test large exponent: frexp(1e+308)") {
    int exp;
    double x = 1e+308;
    double mantissa = frexp(x, &exp);

    test_assert(mantissa >= 0.5 && mantissa < 1.0);
    test_assert(exp == 1024);  // 2^1024 ≈ 1.7e+308
    test_assert(validate(x, mantissa, exp));
}