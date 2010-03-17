/**
 * @file
 *
 * @brief Test checks possibility soft traps calling
 *
 * @date 26.06.2009
 * @author Alexander Batyukov
 */

#include <types.h>
#include <common.h>
#include <embox/test.h>
#include <asm/test/testtraps_core.h>
#include <hal/traps_core.h>

#define TEST_SOFT_TRAP_NUMBER 0x10

EMBOX_TEST(run);

static unsigned int volatile test_variable;
/* test handler*/
static void test_handler(void *data) {
	test_variable++;
}

static int run(void) {
	unsigned int temp = test_variable;

	traps_save_table((uint32_t *)testtraps_table);
	testtraps_set_handler(TRAP_TYPE_SOFTTRAP, TEST_SOFT_TRAP_NUMBER,
			test_handler);

	testtraps_fire_softtrap(TEST_SOFT_TRAP_NUMBER);

	if (temp != (test_variable - 1)) {
		TRACE("Incorrect software traps handling\n");
		return -1;
	}
	traps_restore_table((uint32_t *)testtraps_table);

	return 0;
}
