/**
 * @file
 * @brief Test unit for stdlib/strtof.
 *
 * @date Mar 25, 2026
 * @author Matt Van Horn
 */

#include <embox/test.h>
#include <stdlib.h>

EMBOX_TEST_SUITE("stdlib/strtof_test");

#define STRTOF_EPSILON 1e-6f

TEST_CASE("strtof basic positive number") {
	float value;
	float diff;

	value = strtof("3.14", NULL);
	diff = value - 3.14f;
	if (diff < 0.0f) {
		diff = -diff;
	}

	test_assert(diff < STRTOF_EPSILON);
}

TEST_CASE("strtof negative number") {
	float value;
	float diff;

	value = strtof("-2.5", NULL);
	diff = value - (-2.5f);
	if (diff < 0.0f) {
		diff = -diff;
	}

	test_assert(diff < STRTOF_EPSILON);
}

TEST_CASE("strtof zero") {
	float value;

	value = strtof("0.0", NULL);

	test_assert(value == 0.0f);
}

TEST_CASE("strtof endptr test") {
	char *endptr;
	float value;
	float diff;

	value = strtof("1.5abc", &endptr);
	diff = value - 1.5f;
	if (diff < 0.0f) {
		diff = -diff;
	}

	test_assert(diff < STRTOF_EPSILON);
	test_assert(*endptr == 'a');
	test_assert(endptr[1] == 'b');
	test_assert(endptr[2] == 'c');
	test_assert(endptr[3] == '\0');
}

TEST_CASE("strtof leading whitespace") {
	float value;
	float diff;

	value = strtof("  42.0", NULL);
	diff = value - 42.0f;
	if (diff < 0.0f) {
		diff = -diff;
	}

	test_assert(diff < STRTOF_EPSILON);
}
