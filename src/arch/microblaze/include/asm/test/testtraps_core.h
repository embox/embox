/**
 * @file
 *
 * @data 15.03.2010
 * @author Anton Bondarev
 */

#ifndef TESTTRAPS_H_
#define TESTTRAPS_H_

#include <types.h>
#include <asm/traps_core.h>

extern uint8_t tests_trap_table[TRAP_TABLE_SIZE];

extern void testtraps_set_handler(uint32_t type, int number, uint32_t handler);

extern int testtraps_fire_softtrap(uint32_t uint8_t);

#endif /* TESTTRAPS_H_ */
