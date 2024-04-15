/**
 * @file
 * @brief Microblaze implementation traps framework
 *
 * @date 15.03.10
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <lib/libds/array.h>
#include <asm/msr.h>
#include <hal/env/traps_core.h>
#include <string.h>

static traps_env_t *old_env;
static traps_env_t cur_env;

void traps_enable(void) {
	msr_set_ee();
}

void traps_disable(void) {
	msr_clr_ee();
}

static inline void traps_status_save(uint32_t *status) {
	*status = msr_get_value() & MSR_EE_MASK;
}

static inline void traps_status_restore(uint32_t *status) {
	(*status & MSR_EE_MASK) ? msr_set_ee() :
				    msr_clr_ee();
}

void traps_save_env(traps_env_t *env) {
	if (NULL != old_env) {
		env->base_addr = 0; /*always 0*/
		memcpy(cur_env.hw_traps, hwtrap_handler,
				ARRAY_SIZE(cur_env.hw_traps));
		memcpy(cur_env.soft_traps, sotftrap_handler,
				ARRAY_SIZE(cur_env.soft_traps));
		old_env = &cur_env;
	}
	cur_env.status = msr_get_value();
	memcpy(env, old_env, sizeof(*env));
}

void traps_restore_env(traps_env_t *env) {
	uint32_t msr;
	memcpy(&cur_env, env, sizeof(*env));
	old_env = &cur_env;
	/*save msr register*/
	msr = msr_get_value();
	/*disable traps and interrupts*/
	msr_set_value(msr & ~(MSR_EE_MASK | MSR_IE_MASK));

	/*restore traps status*/
	msr_set_value(msr);
}

void traps_set_env(traps_env_t *env) {
	uint32_t msr;
	memcpy(&cur_env, env, sizeof(*env));
	old_env = &cur_env;
	/*save msr register*/
	msr = msr_get_value();
	/*disable traps and interrupts*/
	msr_set_value(msr & ~(MSR_EE_MASK | MSR_IE_MASK));

	/*restore traps status*/
	msr_set_value(msr);
}
