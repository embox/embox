/**
 * \file test_soft_traps.c
 *
 * \date 26.06.2009
 * \author sunnya
 */

#include "autoconf.h"
#include "common.h"
#include "express_tests.h"

DECLARE_EXPRESS_TEST("software traps", exec, TEST_SOFT_TRAPS_ON_BOOT_ENABLE, NULL);

extern unsigned int volatile test_soft_traps_variable;
#define TEST_SOFT_TRAP_NUMBER 0x10

static int exec(int argc, char** argv) {
	unsigned int temp = test_soft_traps_variable;
	// TODO remove it from here -- Eldar
	__asm__ __volatile__ ("ta %0\n\t"::"i" (TEST_SOFT_TRAP_NUMBER));

	if (temp != (test_soft_traps_variable - 1)) {
		TRACE("Incorrect software traps handling\n");
		return -1;
	}
	return 0;
}
