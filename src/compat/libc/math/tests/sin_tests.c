#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("sin() tests");

TEST_CASE("Test for sin(0)")
{
        test_assert(sin(0) == 0);
}

TEST_CASE("Test for sin(Pi/6)")
{
        test_assert(sin(M_PI/6) == 0.5);
}

TEST_CASE("Test for sin(Pi/2)")
{
        test_assert(sin(M_PI/2) == 1);
}

TEST_CASE("Test for sin(Pi)")
{
        test_assert(sin(M_PI) == 0);
}

TEST_CASE("Test for sin(0)")
{
        test_assert(sin(-0) == 0);
}

TEST_CASE("Test for sin(+INFINITY)")
{
        test_assert(isnan(sin(INFINITY)));
}

TEST_CASE("Test for sin(NAN)")
{
        test_assert(isnan(sin(NAN)));
}
