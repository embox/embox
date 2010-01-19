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
#include "common.h"
#include "express_tests.h"

DECLARE_EXPRESS_TEST("recursion", "rec", exec, TEST_RECURSION_ON_BOOT_ENABLE, NULL);

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
