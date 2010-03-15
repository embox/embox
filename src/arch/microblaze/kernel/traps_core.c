/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#include <asm/traps_core.h>

void traps_enable(void) {
}

void traps_disable(void) {
}

void traps_status_save(uint32_t *status) {

}

void traps_status_restore(uint32_t *status) {

}

void traps_save_table(uint32_t *table) {
	memcpy(table, 0, TRAP_TABLE_SIZE);
}

void traps_restore_table(uint32_t *table) {
	/*disable traps*/
	memcpy(0, table, TRAP_TABLE_SIZE);
	/*restore traps status*/
}
