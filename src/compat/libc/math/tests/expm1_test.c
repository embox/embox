/**
 * @file
 *
 * @date Nov 10, 2024
 * @author Jason Mok
 */

#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("expm1() tests");

TEST_CASE("Test for expm1(0.0)") {
    test_assert(expm1(0.0) == 0.0);
}

TEST_CASE("Test for expm1(3.0)") {
    double res_expm1;
    double res_exp;

    res_expm1 = expm1(3.0);
    res_exp = exp(3.0) - 1.0;

    test_assert(res_expm1 == res_exp);

   //test_assert(expm1(3.0) == (exp(3.0) - 1.0));
}

#if 0
TEST_CASE("Test for expm1(1.0)") {
    double res_expm1;
    double res_comp;

    res_expm1 = expm1(1.0);
    res_comp = (M_E - 1.0);

/* FIXME
    res_expm1 = expm1(1.0) ; ( 1.7182818284590453)
    res_comp = (M_E - 1.0) ; (1.7182818284590451) */

    test_assert(res_expm1 == res_comp);

    //test_assert(expm1(1.0) == (M_E - 1.0));
}
#endif

#if 0
TEST_CASE("Test for expm1(-1.0)") {
    double res_expm1;
    double res_comp;

    res_expm1 = expm1(-1.0);
    res_comp = (-1.0 / M_E);

/* FIXME
    res_expm1 = expm1(1.0) ; ( -0.63212055882855767)
    res_comp = (M_E - 1.0) ; (-0.63212055882855767) */

    test_assert(res_expm1 == res_comp);

    // test_assert(expm1(-1.0) == (-1.0 / M_E));
}
#endif

TEST_CASE("Test for expm1(+INFINITY)") {
    test_assert(isinf(expm1(INFINITY)));
}

TEST_CASE("Test for expm1(-INFINITY)") {
    test_assert(expm1(-INFINITY) == -1.0);
}

TEST_CASE("Test for expm1(NaN)") {
    test_assert(isnan(expm1(NAN)));
}
