/**
 * @file
 *
 * @date October 16, 2024
 * @author Diamond Rivero
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("copysign() tests");

TEST_CASE("Test for copysign(+1.0, +2.0)") {
	test_assert(copysign(1.0, 2.0) == 1.0);
}

TEST_CASE("Test for copysign(+1.0, -2.0)") {
	test_assert(copysign(1.0, -2.0) == -1.0);
}

TEST_CASE("Test for copysign(-10.0, -1.0)") {
	test_assert(copysign(-10.0, -1.0) == -10.0);
}

TEST_CASE("Test for copysign(-10.0, +1.0)") {
	test_assert(copysign(-10.0, 1.0) == 10.0);
}

TEST_CASE("Test for copysign(+8.0, -1.0)") {
	test_assert(copysign(8.0, -1.0) == -8.0);
}

TEST_CASE("Test for copysign(+INFINITY, -2.0)") {
	test_assert(copysign(INFINITY, -2.0) == -INFINITY);
}
#if 0
TEST_CASE("Test for copysign(NAN, -2.0)") {
	test_assert(copysign(NAN, -2.0) == -NAN);
}
#endif
TEST_CASE("Test for copysign(2.0, -0.0)") {
	test_assert(copysign(2.0, -0.0) == -2.0);
}
