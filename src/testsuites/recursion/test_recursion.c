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

static int foo(int depth) {
	if (depth > 0) {
		return foo(--depth) - 1;
	}
	return RECURSION_DEPTH;
}

static int run() {
	if (foo(RECURSION_DEPTH) == 0) {
		return 0;
	} else {
		return -1;
	}
}
