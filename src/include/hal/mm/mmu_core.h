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
#define MMU_PAGE_EXECUTEABLE  0x4


typedef __mmu_paddr_t paddr_t;
typedef __mmu_vaddr_t vaddr_t;

/** pgd - page global directory (page table for specific process) */
typedef __mmu_pgd_t mmu_pgd_t;



/**
 * Defines type for structure of MMU environment. This structure must be
 * describe in platform specific part in file <asm/mmu_core.h>.
 */
typedef __mmu_env_t mmu_env_t;

typedef __mmu_ctx_t mmu_ctx_t;

#define MMU_RRTURN_ERROR     (mmu_ctx)(-1)

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
 *
 * @param phy_addr - starting physic address for memory region
 * @param phy_addr - starting physic address for memory region
 * @param virt_addr - wishing virtual address for memory region
 * @param reg_size - wishing size of mapping region
 *
 * @return size of mapped region
 * @retval size of mapped region on success
 * @retval -1 on fail;
 */
extern int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, uint32_t flags);

/**
 * Switches current MMU context, changes active pgd therefore memory mapping.
 * This is used during switching active process
 */
extern void switch_mm(mmu_ctx_t prev, mmu_ctx_t next);

/**
 * Allocates new pgd table.
 *
 * @return new MMU context
 * @retval new MMU context on success
 * @retval -1 on failed
 */
extern  mmu_ctx_t mmu_create_context(void);

/**
 * Delete pgd and all nested table from mmu tables
 *
 * @param ctx MMU context has been created @link #mmu_create_context() @endlink
 */
extern void mmu_delete_context(mmu_ctx_t ctx);

#endif /* MMU_CORE_H_ */
