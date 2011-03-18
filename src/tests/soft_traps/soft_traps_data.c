/**
 * @file
 * @brief Test checks correctness of transmission parameters during soft trap's
 *        processing
 *
 * @date 26.06.09
 * @author Alexander Batyukov
 */

#include <types.h>
#include <embox/test.h>
#include <hal/test/traps_core.h>

#define TEST_SOFT_TRAP_NUMBER 0x10

EMBOX_TEST(run);

/**
 *  Test handler
 *  */
static int test_handler(uint32_t trap_nr, void *data) {
	(*(uint32_t *) data)++;
	return 0;
}

static int run(void) {
	traps_env_t old_env;
	unsigned int volatile test_variable;
	unsigned int temp = test_variable;

	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	testtraps_set_handler(TRAP_TYPE_SOFTTRAP, TEST_SOFT_TRAP_NUMBER,
			test_handler);

	testtraps_fire_softtrap(TEST_SOFT_TRAP_NUMBER, (void *) &test_variable);

	traps_restore_env(&old_env);

	if (temp != (test_variable - 1)) {
		TRACE("Incorrect software traps handling\n");
		return -1;
	}

	return 0;
}
