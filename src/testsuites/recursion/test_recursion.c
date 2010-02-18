/**
 * @file test_recursion.c
 *
 * @brief tests proper stack usage (particularly, right handling
 * of window overflow//underflow on SPARC).
 *
 * @date 14.08.2009
 * @author Eldar Abusalimov
 */

#include "autoconf.h"
#include "types.h"
#include "common.h"
#include "express_tests.h"

//DECLARE_EXPRESS_TEST(RECURSION, exec, NULL);
DECLARE_EXPRESS_TEST(recursion, exec, NULL);

#define RECURSION_DEPTH 32

static int foo(int depth) {
	if (depth > 0) {
		return foo(--depth);
	}
	return 0;
}

static int exec(int argc, char** argv) {
	return foo(RECURSION_DEPTH);
}
