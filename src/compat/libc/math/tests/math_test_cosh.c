#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("cosh() tests");

static bool is_close(double x, double y) {
	return fabs(x - y) <= 1e-9 * fmin(fabs(x), fabs(y));
}


TEST_CASE("tests for cosh(0.0)") {
	test_assert(cosh(0.0) == 1.0);
}

TEST_CASE("tests for cosh(-0.0)") {
	test_assert(cosh(-0.0) == 1.0);
}

TEST_CASE("tests for cosh(NAN)") {
	test_assert(isnan(cosh(NAN)));
}

TEST_CASE("test for cosh(INFINITY)") {
	test_assert(isinf(cosh(INFINITY)));
}

TEST_CASE("test for cosh(x)") {
	double test_num;
	double cosh_expform;
	int round;

	for (round = 1, test_num = 10.0; round < 100; round++) {
		cosh_expform = (exp(test_num) + exp(-test_num)) / 2;
		test_assert(is_close(cosh(test_num), cosh_expform));
		test_num += 3.0;
	}
}

TEST_CASE("test for cosh parity") {
	test_assert(is_close(cosh(10.0), cosh(-10.0)));
}
