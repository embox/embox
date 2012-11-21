/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#ifndef SPARC_MMU_H_
#define SPARC_MMU_H_

#ifndef __ASSEMBLER__

#include <types.h>

#define __MMU_PGD_SHIFT         24
#define __MMU_PMD_SHIFT         18
#define __MMU_PTE_SHIFT         12

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

#endif

#endif /* SPARC_MMU_H_ */
