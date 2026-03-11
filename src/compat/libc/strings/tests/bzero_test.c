/**
 * @file
 * @brief Test of bzero() function.
 *
 * @date Mar 2026
 */

#include <string.h>
#include <strings.h>
#include <embox/test.h>

#define BZERO_LARGE_BUF_LEN 1024

EMBOX_TEST_SUITE("bzero test");

TEST_CASE("bzero basic") {
	char buf[] = {'a', 'b', 'c', 'd'};

	bzero(buf, sizeof(buf));

	for (size_t i = 0; i < sizeof(buf); i++) {
		test_assert_equal(buf[i], 0);
	}
}

TEST_CASE("bzero partial") {
	char buf[] = {'a', 'b', 'c', 'd', 'e'};
	const size_t n = 3;

	bzero(buf, n);

	for (size_t i = 0; i < n; i++) {
		test_assert_equal(buf[i], 0);
	}

	test_assert_equal(buf[3], 'd');
	test_assert_equal(buf[4], 'e');
}

TEST_CASE("bzero zero length") {
	char buf[] = {'a', 'b', 'c'};

	bzero(buf, 0);

	test_assert_equal(buf[0], 'a');
	test_assert_equal(buf[1], 'b');
	test_assert_equal(buf[2], 'c');
}

TEST_CASE("bzero single byte") {
	char buf[] = {'x'};

	bzero(buf, 1);

	test_assert_equal(buf[0], 0);
}

TEST_CASE("bzero full buffer") {
	char buf[] = {'a', 'b', 'c', 'd', 'e'};

	bzero(buf, sizeof(buf));

	for (size_t i = 0; i < sizeof(buf); i++) {
		test_assert_equal(buf[i], 0);
	}
}


TEST_CASE("bzero large buffer") {
	static char buf[BZERO_LARGE_BUF_LEN];

	memset(buf, 'A', sizeof(buf));

	bzero(buf, sizeof(buf));

	for (size_t i = 0; i < sizeof(buf); i++) {
		test_assert_equal(buf[i], 0);
	}
}
