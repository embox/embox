/**
 * @file
 *
 * @date 10.03.10
 * @author Anton Bondarev
 */

#include <types.h>
#include <hal/test/traps_core.h>

static traps_env_t test_env[1];

//FIXME testtraps_set_handler haven't been implemented yet
void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler) {
	switch(type) {
	case TRAP_TYPE_HARDTRAP:
		hwtrap_handler[number] = handler;
		break;
	case TRAP_TYPE_INTERRUPT:
		break;
	case TRAP_TYPE_SOFTTRAP:
		sotftrap_handler[number] = handler;
		break;
	default:
		/*ERROR: unknown type*/
		return;
	}
}

int testtraps_fire_softtrap(uint32_t number, void *data) {
	/*pass trap number through r5 and data through r6*/
#if 0
	__asm__ __volatile__ (
		"bralid r16, 0x8;\n\t"
		"nop;\n\t"
		"nop;\n\t"
	);
#else
	__asm__ __volatile__ (
		/* we have already loaded parameters
		 "addi  r5, %0, 0;\n\t"
		"addi  r6, %1, 0;\n\t"
		*/
		"brki  r16, 0x8;\n\t"
		"nop;\n\t"
		"nop;\n\t"
		/*: for loaded parametrs
		:"r" (number), "r"(data)
		*/
	);
#endif
	return 0;
}

traps_env_t *testtraps_env(void) {
	return test_env;
}
