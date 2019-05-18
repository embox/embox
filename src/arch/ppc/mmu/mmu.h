/**
 * @file
 * @brief
 *
 * @date 13.12.12
 * @author Ilia Vaprol
 */

#ifndef PPC_MMU_H_
#define PPC_MMU_H_

#include <stdint.h>

#define __MMU_PGD_SHIFT         22
#define __MMU_PMD_SHIFT         22
#define __MMU_PTE_SHIFT         12

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_reg_t;

#endif /* PPC_MMU_H_ */
