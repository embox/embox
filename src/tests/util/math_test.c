/**
 * @file
 * @brief
 *
 * @date 03.11.11
 * @author Anton Kozlov
 * @author Malkovsky Nikolay
 */

#include <embox/test.h>
#include <util/math.h>


EMBOX_TEST_SUITE("math library");

TEST_CASE("Check function for calculate binary logorithm") {
	for(int i = 0; i < 32; ++i) {
		test_assert_equal(blog2(1 << i), i);
	}
	while(1);
}
