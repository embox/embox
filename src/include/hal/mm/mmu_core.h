/**
 * @file
 *
 * @brief Specifies MMU interface.
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#ifndef MMU_CORE_H_
#define MMU_CORE_H_

#include <asm/mmu_core.h>


#define MMU_PAGE_CACHEABLE    0x1
#define MMU_PAGE_WRITEABLE    0x2
#define MMU_PAGE_EXECUTEABLE   0x4


/**
 * Defines type for structure of MMU environment. This structure must be
 * describe in platform specific part in file <asm/mmu_core.h>.
 */
typedef __mmu_env_t mmu_env_t;

/**
 * Setup pointed MMU environment to system.
 *
 * @param env - MMU's environment you want set as current.
 */
extern void mmu_set_env(mmu_env_t *env);

/**
 * Saves current MMU environment. Saved environment can be restore by
 * @link #mmu_restore_env @endlink function. If you want setting own traps
 * handler you must use also @link #traps_save_env @endlink and other
 * function for work with the trap's environment.
 *
 * @param env  - buffer for saving current MMU's environment.
 */
extern void mmu_save_env(mmu_env_t *env);

/**
 * Restores saved MMU's environment. Setups pointed MMU's environment as
 * current. The environment must be saved by @link #mmu_save_env @endlink.
 *
 * @param env - MMU's environment you want set as current.
 */
extern void mmu_restore_env(mmu_env_t *env);

/**
 * Switches on virtual mode in system.
 */
extern void mmu_on(void);

/**
 * Switches off virtual mode in system. Switches on real mode.
 */
extern void mmu_off(void);

/**
 * Maps physic memory region on virtual memory. This function inserts one or
 * more records to mmu tables.
 * Inserting occurs for current process id.
 * Addresses and region's size will be align on mmu page size.
 *
 * @param phy_addr - starting physic address for memory region
 * @param virt_addr - wishing virtual address for memory region
 * @param reg_size - wishing size of mapping region
 *
 * @return size of mapped region
 * @retval size of mapped region on success
 * @retval -1 on fail;
 */
extern int mmu_map_region(uint32_t phy_addr, uint32_t virt_addr,
		size_t reg_size, uint32_t flags);

#endif /* MMU_CORE_H_ */
