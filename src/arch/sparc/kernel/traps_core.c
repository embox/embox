/**
 * @file
 * @brief Sparc implementation traps framework
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#include <string.h>
#include <hal/ipl.h>
#include <asm/tbr.h>
#include <hal/env/traps_core.h>


void traps_enable(void) {

}

void traps_disable(void) {

}

void traps_save_env(traps_env_t *env) {
	env->base_addr = tbr_tba_get();
}

void traps_restore_env(traps_env_t *env) {
	ipl_t ipl_status = ipl_save();

	/* atomic */
	tbr_tba_set(env->base_addr);

	ipl_restore(ipl_status);
}

void traps_set_env(traps_env_t *env) {
	ipl_t ipl_status = ipl_save();

	/* atomic */
	tbr_tba_set(env->base_addr);

	ipl_restore(ipl_status);
}
