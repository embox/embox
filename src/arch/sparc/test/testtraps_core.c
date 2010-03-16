/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <asm/test/testtraps_core.h>

uint8_t teststraps_table[TRAP_TABLE_SIZE];

void testtraps_set_handler(uint32_t type, int number, uint32_t handler) {

}

int testtraps_fire_softtrap(uint32_t number) {
	/*__asm__ __volatile__ ("ta %0\n\t"::"i" (number)); */
	return 0;
}

