/**
 
*@file
 
 
*@date october 12,2024
*@author Abhinav Ananthu
 
*/
 
 #include <math.h>
 #include <embox/test.h>
 
 EMBOX_TEST_SUITE("cbrt()tests");
 
 TEST_CASE("Test for cbrt(8.0)") {
 
    test_assert(cbrt(8.0) ==2.0);
}
 
 TEST_CASE("Test for cbrt(+INFINITY)"){
    test_assert(isinf(cbrt(INFINITY)));
}
 
 TEST_CASE("Test for cbrt(NaN)") {
 
    test_assert(isnan(cbrt(cbrt(NAN))));
}
 
 TEST_CASE("Test for cbrt(0.0)") {
    test_assert(cbrt(0.0) == 0.0);
}
 
 TEST_CASE("Test for cbrt(-0.0)") {
    test_assert(cbrt(-0.0) ==0.0);
}
 
 TEST_CASE("Test for cbrt(-8.0)") {
    test_assert(cbrt(-8.0) == -2.0);
}
 
 TEST_CASE("Test for cbrt(1.0)") {
    test_assert(cbrt(1.0) ==1.0);
}

