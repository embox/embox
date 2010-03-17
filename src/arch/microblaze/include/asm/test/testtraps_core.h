/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef TESTTRAPS_H_
#define TESTTRAPS_H_

#include <types.h>
#include <asm/traps_core.h>

#define TRAP_TYPE_HARDTRAP  0
#define TRAP_TYPE_INTERRUPT 1
#define TRAP_TYPE_SOFTTRAP  2

typedef void (*softtrap_handler)(void *data);

extern uint8_t testtraps_table[TRAP_TABLE_SIZE];

extern void testtraps_set_handler(uint32_t type, int number, softtrap_handler handler);

extern int testtraps_fire_softtrap(uint32_t uint8_t);

#endif /* TESTTRAPS_H_ */
