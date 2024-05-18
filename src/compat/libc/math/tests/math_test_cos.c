#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("cos() tests");

TEST_CASE("Test for cos(0)")
{
        test_assert(cos(0) == 1);
}

TEST_CASE("Test for cos(Pi/3)")
{
        test_assert(cos(M_PI/3) == 0.5);
}

TEST_CASE("Test for cos(Pi/2)")
{
        test_assert(cos(M_PI/2) == 0);
}

TEST_CASE("Test for cos(Pi)")
{
        test_assert(cos(M_PI) == -1);
}

TEST_CASE("Test for cos(-0)")
{
        test_assert(cos(-0) == 1);
}

TEST_CASE("Test for cos(+INFINITY)")
{
        test_assert(isnan(cos(INFINITY)));
}

TEST_CASE("Test for cos(NAN)")
{
        test_assert(isnan(cos(NAN)));
}
