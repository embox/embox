/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <hal/mm/mmu_core.h>
#include <test/testmmu_core.h>

static mmu_env_t env;
mmu_env_t *testmmu_env(void) {
	return &env;
}

