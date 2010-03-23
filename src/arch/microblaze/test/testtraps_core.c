/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <hal/traps_core.h>
#include <test/testtraps_core.h>

static traps_env_t test_env[1];

//FIXME testtraps_set_handler haven't been implemented yet
void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler) {
	switch(type) {
	case TRAP_TYPE_HARDTRAP: {
		break;
	}
	case TRAP_TYPE_INTERRUPT: {
		break;
	}
	case TRAP_TYPE_SOFTTRAP: {
		sotftrap_handler[number] = handler;
		break;
	}
	default:
		/*ERROR: unknown type*/
		return;
	}

}

int testtraps_fire_softtrap(uint32_t number) {
	/*TODO microblaze fire_softtrap not set trap's number*/
#if 0
	__asm__ __volatile__ (
			"bralid r15, 0x8;\n\t"
			"nop;\n\t"
			"nop;\n\t");
#endif
	__asm__ __volatile__ (
			"brki  r16, 0x8;\n\t"
			"nop;\n\t"
			"nop;\n\t");

	return 0;
}

traps_env_t *testtraps_env(void) {
	return test_env;
}
