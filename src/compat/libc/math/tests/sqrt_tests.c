/**
 * @file
 *
 * @date May 25, 2023
 * @aothor Anton Bondarev
 */



#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("sqrt() tests");


TEST_CASE("Test for sqrt() with negative argument") {
	test_assert(isnan(sqrt(-1.0)));
}

TEST_CASE("Test for sqrt(4.0)") {
	test_assert(sqrt(4.0) == 2.0);
}

