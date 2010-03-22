/**
 * @file
 *
 * @brief Microblaze implementation traps framework
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#include <asm/msr.h>
#include <hal/traps_core.h>
#include <string.h>


void traps_enable(void) {
	msr_set_bit(MSR_EE_BIT);
}

void traps_disable(void) {
	msr_clr_bit(MSR_EE_BIT);}

static inline void traps_status_save(uint32_t *status) {
	*status = msr_get_value() & MSR_EE_MASK;
}

static inline void traps_status_restore(uint32_t *status) {
	(*status & MSR_EE_MASK) ? msr_set_bit(MSR_EE_BIT) : msr_clr_bit(MSR_EE_BIT);
}

void traps_save_env(traps_env_t *env) {
	traps_status_save(&env->status);
	env->base_addr = 0; /*always 0*/
}

void traps_restore_env(traps_env_t *env) {
	uint32_t msr;
	/*save msr register*/
	msr = msr_get_value();
	/*disable traps and interrupts*/
	msr_set_value(msr & ~(MSR_EE_MASK | MSR_IE_MASK));

	/*restore traps status*/
	msr_set_value(msr);
}

void traps_set_env(traps_env_t *env) {
/*	uint32_t *trap_table_base = 0;*/
	uint32_t msr;
	/*save msr register*/
	msr = msr_get_value();
	/*disable traps and interrupts*/
	msr_set_value(msr & ~(MSR_EE_MASK | MSR_IE_MASK));

	/*restore traps status*/
	msr_set_value(msr);
}
