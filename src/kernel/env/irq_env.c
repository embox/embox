/**
 * @file
 *
 * @date 30.06.2010
 * @author Anton Bondarev
 */

#include <string.h>
#include <hal/ipl.h>
#include <hal/env/irq_env.h>

static irq_env_t *cur_env;

void irq_save_env(irq_env_t *env) {
	ipl_t ipl;

	ipl = ipl_save();

	memcpy(cur_env, env, sizeof(irq_env_t));

	ipl_restore(ipl);
}

void irq_restore_env(irq_env_t *env) {
	ipl_t ipl;

	ipl = ipl_save();

	memcpy(cur_env, env, sizeof(irq_env_t));

	ipl_restore(ipl);
}

void irq_set_env(irq_env_t *env) {
	cur_env = env;
}

irq_env_t *irq_get_env(void) {
	return cur_env;
}
