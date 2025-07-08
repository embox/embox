
/**
 * @file
 *
 *@date July 7, 2025
 * @author Arofenitra Rarivonjy
 */

 #include <math.h>
 #include <embox/test.h>
 #include <float.h>
 #include <stdbool.h>
 
 EMBOX_TEST_SUITE("erf() tests");
 
 TEST_CASE("Test erf(0.0)"){
     test_assert(erf(0.0) == 0.0);
 }
 
 TEST_CASE("Test erf(-0.0)"){
     test_assert(erf(-0.0) == -0.0);
 }


TEST_CASE("Test erf(1.0)"){
    test_assert(erf(1.0) == 0.842700792949715);
}

TEST_CASE("Test erf(-1.0)"){
    test_assert(erf(-1.0) == -0.842700792949715);
}

TEST_CASE("Test erf(+inf)"){
    test_assert(erf(INFINITY) == 1.0);
}

TEST_CASE("Test erf(-inf)"){
    test_assert(erf(-INFINITY) == -1.0);
}

TEST_CASE("Test erf(nan)"){
    test_assert(isnan(erf(NAN)));
}
/*
Test cases for big numbers, and scaling it
*/
TEST_CASE("Test erf(1e10)"){
    test_assert(erf(1e10) == 1.0);
}

TEST_CASE("Test erf(-1e10)"){
    test_assert(erf(-1e10) == -1.0);
}
