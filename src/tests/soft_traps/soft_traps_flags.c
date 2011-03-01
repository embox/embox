/**
 * @file
 * @brief Test checks possibility soft trap's calling
 *
 * @date 26.06.2009
 * @author Alexander Batyukov
 */

#include <types.h>
#include <embox/test.h>
#include <hal/test/traps_core.h>

#define TEST_SOFT_TRAP_NUMBER 0x10

EMBOX_TEST(run);

static int times = 2;
/* test handler*/
static int test_handler(uint32_t trap_nr, void *data) {
	times--;
	return times;
}

static int run(void) {
	traps_env_t old_env;

	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	testtraps_set_handler(TRAP_TYPE_SOFTTRAP, TEST_SOFT_TRAP_NUMBER,
			test_handler);

	testtraps_fire_softtrap(TEST_SOFT_TRAP_NUMBER, NULL);

	traps_restore_env(&old_env);

	if (times != 0) {
		TRACE("\nIncorrect software traps handling: times = %d\n", times);
		return -1;
	}

	return 0;
}
