/**
 * @file
 *
 * @data May 1, 2024
 * @aothor Zeng Zixian
 */



#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("sinh() tests");

static bool is_close(double x, double y) {
	return fabs(x - y) <= 1e-9 * fmin(fabs(x), fabs(y));
}


TEST_CASE("tests for sinh(0.0)") {
	test_assert(sinh(0.0) == 0.0);
}

TEST_CASE("tests for sinh(-0.0)") {
	test_assert(sinh(-0.0) == -0.0);
}

TEST_CASE("tests for sinh(NAN)") {
	test_assert(isnan(sinh(NAN)));
}

TEST_CASE("test for sinh(INFINITY)") {
	test_assert(isinf(sinh(INFINITY)));
}

TEST_CASE("test for sinh(x)") {
	double test_num;
	double sinh_expform;
	int round;

	for (round = 1, test_num = 10.0; round < 100; round++) {
		sinh_expform = (exp(test_num) - exp(-test_num)) / 2;
		test_assert(is_close(sinh(test_num), sinh_expform));
		test_num += 3.0;
	}
}

TEST_CASE("test for sinh parity") {
	test_assert(is_close(sinh(10.0), -sinh(-10.0)));
}
