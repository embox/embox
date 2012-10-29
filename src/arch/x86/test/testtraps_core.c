/**
 * @file
 *
 * @brief
 *
 * @date 21.09.2011
 * @author Anton Bondarev
 */

#include <hal/test/traps_core.h>

static traps_env_t test_env[1];

extern __trap_handler __exception_table[0x20];

void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler) {
	if(TRAP_TYPE_HARDTRAP == type) {
		test_env[0].hw_traps[number] = handler;
		__exception_table[number] = handler;
	}

}

int testtraps_fire_softtrap(uint32_t number, void *data) {
	return 0;
}

traps_env_t *testtraps_env(void) {
	return test_env;
}
