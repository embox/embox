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

EMBOX_TEST(math_test);

static int math_test(void) {
	int result = 0;

        for(int i = 0; i < 32; ++i) {
                if(i != blog2(1 << i)) {
                        result += 1;
                        break;
                }
        }

	return result;
}
