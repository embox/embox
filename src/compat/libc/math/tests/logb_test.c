/**
 * @file
 *
 * @date March 19, 2025
 * @author Shaurya Rane
 */

 #include <math.h>

 #include <embox/test.h>

 EMBOX_TEST_SUITE("acosh() tests");

 TEST_CASE("Test for acosh() with argument less than 1") {
     test_assert(isnan(acosh(0.5)));
 }

 TEST_CASE("Test for acosh(1.0)") {
     test_assert(acosh(1.0) == 0.0);
 }

 // acosh(x) = ln(x + sqrt(x^2 - 1))
 TEST_CASE("Test for acosh(10.0)") {
     test_assert(fabs(acosh(10.0) - log(10.0 + sqrt(10.0 * 10.0 - 1))) < 1e-6);
 }

 TEST_CASE("Test for acosh(+INFINITY)") {
     test_assert(isinf(acosh(INFINITY)));
 }

 TEST_CASE("Test for acosh(NaN)") {
     test_assert(isnan(acosh(NAN)));
 }

 // Taylor series approximation, acosh(x) ≈ sqrt(2 * (x - 1))
 TEST_CASE("Test for acosh(1.0000001)") {
     test_assert(fabs(acosh(1.0000001) - sqrt(2 * (1.0000001 - 1))) < 1e-6);
 }

 TEST_CASE("Test for acosh(0.0)") {
     test_assert(isnan(acosh(0.0)));
 }

 TEST_CASE("Test for acosh(-INFINITY)") {
     test_assert(isnan(acosh(-INFINITY)));
 }