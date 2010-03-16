/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <asm/test/testtraps.h>

uint8_t testtraps_table[TRAP_TABLE_SIZE];

void testtraps_set_handler(uint32_t type, int number, uint32_t handler) {

}

int testtraps_fire_softtrap(uint32_t uint8_t) {
	return 0;
}



