/**
 * @file
 * @brief
 *
 * @date 19.03.26
 * @author preetham-vish 
 */

#include<embox/test.h> 
#include<stdlib.h> 

EMBOX_TEST_SUITE("stdlib/atol");
    
TEST_CASE("Check atol() function") { 
    test_assert_equal(0, atol("0")); 
    test_assert_equal(123, atol("+123")); 
    test_assert_equal(-123, atol("-123"); 
    test_assert_equal(123, atol("00123")); 
    test_assert_equal(123, atol("    123")); 
    test_assert_equal(123, atol("123junk")); 
    test_assert_equal(0, atol("abc")); 
    test_assert_equal(0, atol("00abc123")); 
    test_assert_equal(0, atol("")); 
}


