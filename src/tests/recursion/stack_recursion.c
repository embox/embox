/**
 * @file
 * @brief tests proper stack usage (particularly, right handling
 * of window overflow/underflow on SPARC).
 *
 * @date 14.08.09
 * @author Eldar Abusalimov
 */

#include <embox/test.h>
#include <test/misc.h>

#define RECURSION_DEPTH 32

EMBOX_TEST(run);

static int run(void) {
	return test_misc_recursion(RECURSION_DEPTH);
}
