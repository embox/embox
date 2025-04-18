/**
 * @file
 *
 * @date April 13, 2025
 * @author Atharv Dubey
 */

 #include <math.h>

 #include <embox/test.h>
 
 EMBOX_TEST_SUITE("log1p() tests");
 
 TEST_CASE("Test for log1p() with -1.0 argument") {
     double x = log1p(-1.0);
     test_assert(x == -INFINITY);
 }
 
 TEST_CASE("Test for log1p() with argument less than -1.0") {
     double x = log1p(-1.1);
     test_assert(isnan(x));
 }
 
 TEST_CASE("Test for log1p() with 0.0 argument") {
     double x = log1p(0.0);
     test_assert(x == 0.0);
 }
 
 TEST_CASE("Test for log1p() with -0.0 argument") {
     double x = log1p(-0.0);
     test_assert(x == -0.0);
 }
 
 TEST_CASE("Test for log1p() with +INFINITY argument") {
     double x = log1p(INFINITY);
     test_assert(isinf(x));
 }
 
 TEST_CASE("Test for log1p() with -INFINITY argument") {
     double x = log1p(-INFINITY);
     test_assert(isnan(x));
 }
 
 TEST_CASE("Test for log1p() with NaN argument") {
     double x = log1p(NAN);
     test_assert(isnan(x));
 }
 
 TEST_CASE("Test for log1p(e - 1)") {
     double x = log1p(M_E - 1);
     test_assert(x == 1.0);
 }
 
 TEST_CASE("Test for log1p(0.71828)") {
     double x = log1p(0.71828); // log(1.71828)
     test_assert(fabs(x - log(1.71828)) < 1e-12);
 }
 
 TEST_CASE("Test for log1p(-0.63212)") {
     double x = log1p(-0.63212); // log(1 - 0.63212)
     test_assert(fabs(x - log(0.36788)) < 1e-12);
 }
 