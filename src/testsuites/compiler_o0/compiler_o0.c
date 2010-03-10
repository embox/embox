/**
 * \file compiler_o0.c
 *
 * \date 10.03.2010
 * \author Alexey Fomin
 */

#include <express_tests.h>

DECLARE_EXPRESS_TEST(compiler_o0, compiler_o0_test, NULL);


/* That's a temporary test for microblaze only
 * (for other archs it's always correct and meaningless)
 * it fails when -o0 is set for compiler
 * In that case nothing works correctly =/ */
static int compiler_o0_test(int argc, char** argv) {
	int a = 1;

	a = a + 1;

	/* uncommenting this line changes results of the test
	 * when optimization level is o0 */

	/* while(a==1); */

	if(1<<a == 4) {
		return EXPRESS_TESTS_PASSED_RETCODE;
	} else {
		TRACE("\n-o0 test failed, set -o1 to compiler\n");
		EXPRESS_TESTS_FAILED_RETCODE;
	}
}
