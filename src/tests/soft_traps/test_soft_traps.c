/**
 * @file
 *
 * @brief Test checks possibility soft traps calling
 *
 * @date 26.06.2009
 * @author Alexander Batyukov
 */

#include <types.h>
#include <embox/test.h>
#include <test/testtraps_core.h>


#define TEST_SOFT_TRAP_NUMBER 0x10

EMBOX_TEST(run);

static unsigned int volatile test_variable;
/* test handler*/
static void test_handler(uint32_t trap_nr, void *data) {
	test_variable++;
}

static int run(void) {
	unsigned int temp = test_variable;
	traps_env_t old_env;

	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	testtraps_set_handler(TRAP_TYPE_SOFTTRAP, TEST_SOFT_TRAP_NUMBER,
			test_handler);

	testtraps_fire_softtrap(TEST_SOFT_TRAP_NUMBER);

	traps_restore_env(&old_env);

	if (temp != (test_variable - 1)) {
		TRACE("Incorrect software traps handling\n");
		return -1;
	}

	return 0;
}
