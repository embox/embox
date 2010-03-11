/**
 * \file test_soft_traps.c
 *
 * \date 26.06.2009
 * \author sunnya
 */

#include <types.h>
#include <common.h>
#include <embox/test.h>

#define TEST_SOFT_TRAP_NUMBER 0x10

EMBOX_TEST(run);

extern unsigned int volatile test_soft_traps_variable;

static int run(void) {
	unsigned int temp = test_soft_traps_variable;

	trap_fire(TEST_SOFT_TRAP_NUMBER);

	if (temp != (test_soft_traps_variable - 1)) {
		TRACE("Incorrect software traps handling\n");
		return -1;
	}
	return 0;
}
