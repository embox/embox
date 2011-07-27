/**
 * @file
 *
 * @date 15.03.10
 * @author Anton Bondarev
 */

#ifndef TEST_MMU_CORE_H_
#define TEST_MMU_CORE_H_

#include <types.h>
#include <hal/mm/mmu_core.h>
#include <asm/hal/test/mmu_core.h>

/**
 * Initializes testing mmu environment.
 *
 * @return testing mmu environment
 * @retval pointer to mmu_env structure on success
 * @retval NULL on fail
 */
extern mmu_env_t *testmmu_env(void);

#endif /* TEST_MMU_CORE_H_ */
