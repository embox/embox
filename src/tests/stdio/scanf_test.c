/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.04.2013
 */

#include <stdio.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("stdio/scanf test");

TEST_CASE("sscanf should accept fmt with some placeholders") {
	const char *data = "1234 5678";
	int a = 0, b = 0;

	test_assert(sscanf(data, "%d %d", &a, &b) > 0);

	test_assert_equal(a, 1234);
	test_assert_equal(b, 5678);
}


