/**
 * \file test_soft_traps.c
 *
 * \date 26.06.2009
 * \author sunnya
 */

#include <types.h>
#include <common.h>
#include <express_tests.h>

DECLARE_EXPRESS_TEST(soft_traps, exec, NULL);

extern unsigned int volatile test_soft_traps_variable;
#define TEST_SOFT_TRAP_NUMBER 0x10

static int exec(int argc, char** argv) {
	unsigned int temp = test_soft_traps_variable;

	trap_fire(TEST_SOFT_TRAP_NUMBER);

	if (temp != (test_soft_traps_variable - 1)) {
		TRACE("Incorrect software traps handling\n");
		return EXPRESS_TESTS_FAILED_RETCODE;
	}
	return EXPRESS_TESTS_PASSED_RETCODE;
}
