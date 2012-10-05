/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#ifndef SPARC_MMU_H_
#define SPARC_MMU_H_

#include <types.h>

#define __MMU_PGD_SHIFT         24
#define __MMU_PMD_SHIFT         18
#define __MMU_PTE_SHIFT         12

#define MMU_PAGE_WRITABLE       __MMU_PAGE_WRITABLE
#define MMU_PAGE_USER_MODE      __MMU_PAGE_USER_MODE
#define MMU_PAGE_CACHABLE       __MMU_PAGE_CACHABLE

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_page_flags_t;

#endif /* SPARC_MMU_H_ */
