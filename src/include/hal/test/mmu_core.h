/**
 * @file
 *
 * @date 15.03.2010
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

/**
 * Need for mmu data and instructions miss testing
 */
#define VADDR(phyaddr) (0xf0000000 + ((uint32_t)(phyaddr) - (uint32_t)(&_data_start)))

#endif /* TEST_MMU_CORE_H_ */
