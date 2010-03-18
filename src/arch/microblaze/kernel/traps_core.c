/**
 * @file
 *
 * @brief Microblaze implementation traps framework
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#include <asm/msr.h>
#include <asm/traps_core.h>
#include <string.h>


void traps_enable(void) {
	msr_set_bit(MSR_EE_BIT);
}

void traps_disable(void) {
	msr_clr_bit(MSR_EE_BIT);}

void traps_status_save(uint32_t *status) {
	*status = msr_get_value() & MSR_EE_MASK;
}

void traps_status_restore(uint32_t *status) {
	(*status & MSR_EE_MASK) ? msr_set_bit(MSR_EE_BIT) : msr_clr_bit(MSR_EE_BIT);
}

void traps_save_table(uint32_t *table) {
	const void *trap_table_base = 0;
	memcpy(table, trap_table_base, TRAP_TABLE_SIZE);
}

void traps_restore_table(uint32_t *table) {
/*	uint32_t *trap_table_base = 0;*/
	uint32_t msr;
	/*save msr register*/
	msr = msr_get_value();
	/*disable traps and interrupts*/
	msr_set_value(msr & ~(MSR_EE_MASK | MSR_IE_MASK));
/* we have probler in this place
	memcpy(trap_table_base, table, TRAP_TABLE_SIZE);
*/
	/*restore traps status*/
	msr_set_value(msr);
}
