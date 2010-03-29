/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#include <types.h>
#include <test/testtraps_core.h>
#include <asm/head.h>


static traps_env_t test_env[1];

extern trap_handler_t test_handler[CONFIG_MAX_SOFTTRAP_NUMBER];
extern uint32_t *__test_trap_table;

void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler) {
	switch(type) {
	case TRAP_TYPE_HARDTRAP: {
	}
	case TRAP_TYPE_INTERRUPT: {
	}
	case TRAP_TYPE_SOFTTRAP: {
		test_handler[number] = handler;
	}
	default:
		return;
	}

}

int testtraps_fire_softtrap(uint32_t number, void *data) {
	__asm__ __volatile__ (
			"mov %0, %%o0;\n\t"
			"mov %1, %%o1;\n\t"
			"ta %0;\n\t"
			"nop;\t\n"
			:
			:"r" (number), "r" (data));
	return 0;
}

traps_env_t *testtraps_env(void) {
	test_env[0].base_addr = (uint32_t) &__test_trap_table;
	return test_env;
}
