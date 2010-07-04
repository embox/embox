/**
 * @file
 *
 * @date 04.07.2010
 * @author Anton Bondarev
 */

#ifndef MMU_TYPES_H_
#define MMU_TYPES_H_

#include <asm/hal/mm/mmu_types.h>

typedef __mmu_paddr_t paddr_t;
typedef __mmu_vaddr_t vaddr_t;

typedef __mmu_page_flags_t mmu_page_flags_t;

/** pgd - page global directory (page table for specific process) */
typedef __mmu_pgd_t mmu_pgd_t;

/** pgd - page middle directory (page table for specific process) */
typedef __mmu_pmd_t mmu_pmd_t;
/** pgd - page table entry (page table for specific process) */
typedef __mmu_pte_t mmu_pte_t;

/**
 * Defines type for structure of MMU environment. This structure must be
 * describe in platform specific part in file <asm/hal/mm/mmu_core.h>.
 */
typedef __mmu_env_t mmu_env_t;

typedef __mmu_ctx_t mmu_ctx_t;


#endif /* MMU_TYPES_H_ */
