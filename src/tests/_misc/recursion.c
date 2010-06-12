/**
 * @file
 * @brief Simple recursion
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#include <test/misc.h>

static int call_me(int depth, int total) {
	if (depth > 0) {
		return call_me(--depth, total) - 1;
	}
	return total;
}

int test_misc_recursion(int depth) {
	return call_me(depth, depth);
}
