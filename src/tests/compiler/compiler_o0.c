/**
 * @file
 *
 * @date 10.03.10
 * @author Alexey Fomin
 */

#include <embox/test.h>

EMBOX_TEST(run);

/* That's a temporary test for microblaze only
 * (for other archs it's always correct and meaningless)
 * it fails when -o0 is set for compiler
 * In that case nothing works correctly =/ */
static int run(void) {
	int a = 1;

	a = a + 1;

	/* uncommenting this line changes results of the test
	 * when optimization level is o0 */

	 //while (a==1);

	return 1<<a != 4;
}
