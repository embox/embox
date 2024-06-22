/**
 * @file
 * @brief SPARC specific
 *
 * @date 10.03.10
 * @author Anton Bondarev
 * @author Alexander Batyukov
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/head.h>
#include <hal/test/traps_core.h>

//static traps_env_t test_env[1];
#define CONFIG_MIN_HWTRAP_NUMBER    0x0
#define CONFIG_MIN_INTERRUPT_NUMBER 0x10
#define CONFIG_MIN_SOFTTRAP_NUMBER  0x20

static __trap_handler testtrap_handlers[TRAP_TABLE_SIZE];

void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler) {
	switch (type) {
	case TRAP_TYPE_HARDTRAP:
		assertf(number + CONFIG_MIN_HWTRAP_NUMBER < CONFIG_MIN_INTERRUPT_NUMBER,
		    "hard trap - 0x%x\n", number);
		testtrap_handlers[number + CONFIG_MIN_HWTRAP_NUMBER] = handler;
		break;
	case TRAP_TYPE_INTERRUPT:
		assertf(number + CONFIG_MIN_INTERRUPT_NUMBER < CONFIG_MIN_SOFTTRAP_NUMBER,
		    "irq trap - 0x%x\n", number);
		testtrap_handlers[number + CONFIG_MIN_INTERRUPT_NUMBER] = handler;
		break;
	case TRAP_TYPE_SOFTTRAP:
		assertf(number + CONFIG_MIN_SOFTTRAP_NUMBER < TRAP_TABLE_SIZE,
		    "soft trap - 0x%x\n", number);
		testtrap_handlers[number + CONFIG_MIN_SOFTTRAP_NUMBER] = handler;
		break;
	default:
		return;
	}
}

int testtraps_fire_softtrap(uint32_t number, void *data) {
	__asm__ __volatile__("mov %0, %%o0;\n\t"
	                     "mov %1, %%o1;\n\t"
	                     "ta %0;\n\t"
	                     "nop;\t\n"
	                     :
	                     : "r"(number), "r"(data));
	return 0;
}

traps_env_t *testtraps_env(void) {
	return NULL;
}

int testtrap_handle(uint8_t tt, void *data) {
	if (NULL == testtrap_handlers[tt]) {
		return -1;
	}
	testtrap_handlers[tt](tt, data);
	return 0;
}
