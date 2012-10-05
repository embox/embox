
/**
 * @file
 * @brief
 *
 * @date 04.10.2012
 * @author Anton Bulychev
 */

#ifndef X86_MMU_H_
#define X86_MMU_H_

#include <types.h>

#define __MMU_PGD_SHIFT         22
#define __MMU_PMD_SHIFT         22
#define __MMU_PTE_SHIFT         12

#define __MMU_PAGE_WRITABLE     ((1 << 0) << 1)
#define __MMU_PAGE_USER_MODE    ((1 << 1) << 2)
#define __MMU_PAGE_CACHABLE     (1 << 4)

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_page_flags_t;

#endif /* X86_MMU_H_ */
