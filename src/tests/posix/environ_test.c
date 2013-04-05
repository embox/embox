/**
 * @file
 * @brief
 *
 * @date 28.02.13
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <stdlib.h>
#include <unistd.h>

EMBOX_TEST_SUITE("stdlib/environ api");

#define ENV_NAME "foo"
#define ENV_VAL1  "bar"
#define ENV_VAL2  "baz"
#define ENV_STR1  (ENV_NAME "=" ENV_VAL1)
#define ENV_STR2  (ENV_NAME "=" ENV_VAL2)

TEST_CASE("environ must be null after initializing") {
	test_assert_null(environ);
}

TEST_CASE("getenv() should return null for empty"
		" enviroment") {
	test_assert_null(getenv(ENV_NAME));
}

TEST_CASE("putenv() may put new element of environment") {
	test_assert_zero(putenv(ENV_STR1));
}

TEST_CASE("environ is null-terminated array with added values") {
	test_assert_not_null(environ);
	test_assert_str_equal(environ[0], ENV_STR1);
	test_assert_null(environ[1]);
}

TEST_CASE("getenv() return variable according name") {
	test_assert_str_equal(getenv(ENV_NAME), ENV_VAL1);
}

TEST_CASE("setenv() not overwrite exist value if flag"
		" is not set") {
	test_assert_zero(setenv(ENV_NAME, ENV_VAL2, 0));
	test_assert_str_equal(getenv(ENV_NAME), ENV_VAL1);
}

TEST_CASE("setenv() overwrite exist value if flag"
		" is set") {
	test_assert_zero(setenv(ENV_NAME, ENV_VAL2, 1));
	test_assert_str_equal(getenv(ENV_NAME), ENV_VAL2);
}

TEST_CASE("putenv() overwrite exist value") {
	test_assert_zero(putenv(ENV_STR1));
	test_assert_str_equal(getenv(ENV_NAME), ENV_VAL1);
}

TEST_CASE("unsetenv() may remove exist value") {
	test_assert_not_null(getenv(ENV_NAME));
	test_assert_zero(unsetenv(ENV_NAME));
	test_assert_null(getenv(ENV_NAME));
}

TEST_CASE("environ must be null when environment hasn't"
		" variables") {
	test_assert_null(environ);
}
