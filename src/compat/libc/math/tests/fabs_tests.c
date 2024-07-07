/**
 * @file
 *
 * @date July 07, 2024
 * @author Ankith Veldandi
 */



#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("fabs() tests");

TEST_CASE("Test for fabs(2.33)"){
    test_assert(fabs(2.33) == 2.33);
}

TEST_CASE("Test for fabs(-2.33)"){
    test_assert(fabs(-2.33) == 2.33);
}

TEST_CASE("Test for fabs(-233)"){
    test_assert(fabs(-233) == 233);
}

TEST_CASE("Test for fabs(0.0)"){
    test_assert(fabs(0.00) == 0.0);
}

TEST_CASE("Test for fabs(-0.0)"){
    test_assert(fabs(-0.00) == 0.0);
}

TEST_CASE("Test for fabs(+INFINITY)"){
    test_assert(isinf(fabs(INFINITY)));
}

TEST_CASE("Test for fabs(-INFINITY)"){
    test_assert(isinf(fabs(-INFINITY)));
}

TEST_CASE("Test for fabs(NAN)"){
    test_assert(isnan(fabs(NAN)));
}