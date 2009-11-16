/**
 * @file test_recursion.c
 *
 * @brief tests proper stack usage (particularly, right handling
 * of window overflow//underflow on SPARC).
 *
 * @date 14.08.2009
 * @author Eldar Abusalimov
 */

#include "express_tests.h"

DECLARE_EXPRESS_TEST("recursion", exec);

#define RECURSION_DEPTH 32

static int foo(int depth) {
	if (depth > 0) {
		return foo(--depth);
	}
	return 0;
}

static int exec() {
	return foo(RECURSION_DEPTH);
}
