/**
 * @file
 *
 * @date 09.03.2010
 * @author Anton Bondarev
 */

#include <types.h>

#include <embox/test.h>
#include <hal/traps_core.h>
#include <hal/mm/mmu_core.h>
#include <test/testmmu_core.h>

EMBOX_TEST(run);



static int run() {
	mmu_env_t prev_mmu_env;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());
	mmu_on();

	mmu_restore_env(&prev_mmu_env);
	return 0;
}
