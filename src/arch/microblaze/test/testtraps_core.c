/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <asm/test/testtraps_core.h>

uint8_t testtraps_table[TRAP_TABLE_SIZE];
//FIXME testtraps_set_handler haven't been implemented yet
extern softtrap_handler test_handler;
void testtraps_set_handler(uint32_t type, int number, softtrap_handler handler) {
	switch(type) {
	case TRAP_TYPE_HARDTRAP: {
		break;
	}
	case TRAP_TYPE_INTERRUPT: {
		break;
	}
	case TRAP_TYPE_SOFTTRAP: {
		test_handler = handler;
		break;
	}
	default:
		/*ERROR: unknown type*/
		return;
	}

}
#include <common.h>

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
