/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("math/common routines's tests");

static const double values[] = { 0.0, 0.131321312312, 21.3, 2883321.0 };

TEST_CASE("Test for signbit") {
	size_t i;

	/* signbit */
	for (i = 0; i < sizeof values / sizeof values[0]; ++i) {
		test_assert_zero(signbit(values[i]));
		test_assert_not_zero(signbit(-values[i]));
	}

	/* signbitf */
	for (i = 0; i < sizeof values / sizeof values[0]; ++i) {
		test_assert_zero(signbitf((float)values[i]));
		test_assert_not_zero(signbitf(-(float)values[i]));
	}

	/* signbitl */
	for (i = 0; i < sizeof values / sizeof values[0]; ++i) {
		test_assert_zero(signbitl((long double)values[i]));
		test_assert_not_zero(signbitl(-(long double)values[i]));
	}
}
