/**
 * @file
 * @brief SPARC specific
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#include <types.h>
#include <hal/test/testtraps_core.h>
#include <asm/head.h>

static traps_env_t test_env[1];

void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler) {
	switch(type) {
	case TRAP_TYPE_HARDTRAP:
		if(number < CONFIG_MAX_RESERVED_TRAP) {
			test_handlers[number] = handler;
		}
		if (number >= CONFIG_MIN_HWTRAP_NUMBER && number <= CONFIG_MAX_HWTRAP_NUMBER) {
			test_handlers[number] = handler;
		}
		break;
	case TRAP_TYPE_INTERRUPT:
		test_handlers[number + CONFIG_MIN_INTERRUPT_NUMBER] = handler;
		break;
	case TRAP_TYPE_SOFTTRAP:
		test_handlers[number + CONFIG_MIN_SOFTTRAP_NUMBER] = handler;
		break;
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
		:"r" (number), "r" (data)
	);
	return 0;
}

traps_env_t *testtraps_env(void) {
	test_env[0].base_addr = (uint32_t) &__test_trap_table;
	return test_env;
}
