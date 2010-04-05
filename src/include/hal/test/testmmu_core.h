/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef TESTMMU_CORE_H_
#define TESTMMU_CORE_H_

#include <types.h>
#include <hal/mm/mmu_core.h>
#include <asm/hal/test/testmmu_core.h>

/**
 * Initializes testing mmu environment.
 *
 * @return testing mmu environment
 * @retval pointer to mmu_env structure on success
 * @retval NULL on fail
 */
extern mmu_env_t *testmmu_env(void);

#endif /* TESTMMU_CORE_H_ */
