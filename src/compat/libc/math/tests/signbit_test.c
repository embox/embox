/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("math/common routines's tests");

TEST_CASE("Test for signbit") {
	size_t i;
	const double values[] = {0.0, 0.131321312312, 21.3, 2883321.0};

	/* signbit */
	for (i = 0; i < sizeof values / sizeof values[0]; ++i) {
		test_assert_zero(signbit(values[i]));
		test_assert_not_zero(signbit(-values[i]));
	}

	/* signbitf */
	for (i = 0; i < sizeof values / sizeof values[0]; ++i) {
		test_assert_zero(signbit((float)values[i]));
		test_assert_not_zero(signbit(-(float)values[i]));
	}

	/* signbitl */
	for (i = 0; i < sizeof values / sizeof values[0]; ++i) {
		test_assert_zero(signbit((long double)values[i]));
		test_assert_not_zero(signbit(-(long double)values[i]));
	}
}

TEST_CASE("Test for soft-fp operations") {
	double z, l, m;

	z = 0.0;
	l = -1.3;
	m = 1.3;

	test_assert_true(l == l);
	test_assert_true(-l != l);
	test_assert_true(l != -l);
	test_assert_true(-l == -l);
	test_assert_true(z == z);
	test_assert_true(-z == z);
	test_assert_true(z == -z);
	test_assert_true(-z == -z);

	test_assert_false(l != l);
	test_assert_false(-l == l);
	test_assert_false(l == -l);
	test_assert_false(-l != -l);
	test_assert_false(z != z);
	test_assert_false(-z != z);
	test_assert_false(z != -z);
	test_assert_false(-z != -z);

	test_assert_true(l >= l);
	test_assert_true(z >= l);
	test_assert_true(z >= z);
	test_assert_true(m >= l);
	test_assert_true(m >= z);
	test_assert_true(m >= m);
	test_assert_true(-l <= -l);
	test_assert_true(-z <= -l);
	test_assert_true(-z <= -z);
	test_assert_true(-m <= -l);
	test_assert_true(-m <= -z);
	test_assert_true(-m <= -m);

	test_assert_false(l < l);
	test_assert_false(z < l);
	test_assert_false(z < z);
	test_assert_false(m < l);
	test_assert_false(m < z);
	test_assert_false(m < m);
	test_assert_false(-l > -l);
	test_assert_false(-z > -l);
	test_assert_false(-z > -z);
	test_assert_false(-m > -l);
	test_assert_false(-m > -z);
	test_assert_false(-m > -m);

	test_assert_true(l <= l);
	test_assert_true(l <= z);
	test_assert_true(l <= m);
	test_assert_true(z <= z);
	test_assert_true(z <= m);
	test_assert_true(m <= m);
	test_assert_true(-l >= -l);
	test_assert_true(-l >= -z);
	test_assert_true(-l >= -m);
	test_assert_true(-z >= -z);
	test_assert_true(-z >= -m);
	test_assert_true(-m >= -m);

	test_assert_false(l > l);
	test_assert_false(l > z);
	test_assert_false(l > m);
	test_assert_false(z > z);
	test_assert_false(z > m);
	test_assert_false(m > m);
	test_assert_false(-l < -l);
	test_assert_false(-l < -z);
	test_assert_false(-l < -m);
	test_assert_false(-z < -z);
	test_assert_false(-z < -m);
	test_assert_false(-m < -m);

	test_assert_true(l < z);
	test_assert_true(l < m);
	test_assert_true(z < m);
	test_assert_true(-l > -z);
	test_assert_true(-l > -m);
	test_assert_true(-z > -m);

	test_assert_false(l >= z);
	test_assert_false(l >= m);
	test_assert_false(z >= m);
	test_assert_false(-l <= -z);
	test_assert_false(-l <= -m);
	test_assert_false(-z <= -m);

	test_assert_true(z > l);
	test_assert_true(m > l);
	test_assert_true(m > z);
	test_assert_true(-z < -l);
	test_assert_true(-m < -l);
	test_assert_true(-m < -z);

	test_assert_false(z <= l);
	test_assert_false(m <= l);
	test_assert_false(m <= z);
	test_assert_false(-z >= -l);
	test_assert_false(-m >= -l);
	test_assert_false(-m >= -z);
}
