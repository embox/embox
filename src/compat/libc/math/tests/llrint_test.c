/**
 * @file
 *
 * @date April 05, 2025
 * @author Atharv Dubey
 */

 #include <math.h>
 #include <embox/test.h>
 
 EMBOX_TEST_SUITE("llrint() tests");
 
 TEST_CASE("Test for llrint(0.23)") {
     test_assert(llrint(0.23) == 0LL);
 }
 
 TEST_CASE("Test for llrint(0.73)") {
     test_assert(llrint(0.73) == 1LL);
 }
 
 TEST_CASE("Test for llrint(0.0)") {
     test_assert(llrint(0.0) == 0LL);
 }
 
 TEST_CASE("Test for llrint(3.5)") {
     test_assert(llrint(3.5) == 4LL);
 }
 
 TEST_CASE("Test for llrint(-3.5)") {
     test_assert(llrint(-3.5) == -4LL);
 }
 