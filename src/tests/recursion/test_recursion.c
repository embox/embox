/**
 * @file
 *
 * @brief tests proper stack usage (particularly, right handling
 * of window overflow/underflow on SPARC).
 *
 * @date 14.08.2009
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#define RECURSION_DEPTH 32

EMBOX_TEST(run);
EMBOX_TEST_EXPORT(run_recursion);

static int call_me(int depth) {
	if (depth > 0) {
		return call_me(--depth) - 1;
	}
	return RECURSION_DEPTH;
}

static int run() {
	return call_me(RECURSION_DEPTH);
}
