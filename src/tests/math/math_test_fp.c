/**
 * @math_test_fp
 * @simple tests for fp functions in embox just to make sure the corresponding functions are not stubs
 * @date 03.03.2020
 * @author Rushil kvs
 */

#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("Tests for fp functions");

bool equal(double a, double b)
{
	double epsilon = 0.00000000000001;
	if ((a-b) < epsilon && -(a-b) < epsilon) return true;
  	return false;
}

TEST_CASE("Test for modf") {
	double i,t,j;
	i = modf(2.34, &t);
	j = 0.34;
	test_assert_true(equal(i, j));
	test_assert_true(equal(t, 2.0));
}

TEST_CASE("Test for fmod") {
	double i;
	i = fmod(11, 2.5);
	test_assert_true(equal(i, 1.0));
}

TEST_CASE("Test for signbit") {
	size_t i;
	const double values[] = { 0.0, 0.131321312312, 21.3, 2883321.0 };

	for (i = 0; i < sizeof values / sizeof values[0]; ++i) {
		test_assert_zero(signbit(values[i]));
		test_assert_not_zero(signbit(-values[i]));
	}
}

TEST_CASE("Test for fabs") {
	double i,j;
	i = 8.2;
	j = -0.12;
	test_assert_true(equal(i, fabs(i)));
	test_assert_true(equal(-j, fabs(j)));
}

TEST_CASE("Test for fabsf") {
	float i,j;
	i = 4.6;
	j = -0.56;
	test_assert_true(equal(i, fabsf(i)));
	test_assert_true(equal(-j, fabsf(j)));
}

TEST_CASE("Test for fabsl") {
	long double i,j;
	i = 5.6;
	j = -0.34;
	test_assert_true(equal(i, fabsl(i)));
	test_assert_true(equal(-j, fabsl(j)));
}
