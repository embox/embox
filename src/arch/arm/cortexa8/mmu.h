/**
 * @file mmu.h
 * @brief MMU-trlated macros, functions, etc.
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-08-17
 */

#ifndef ARM_MMU_H_
#define ARM_MMU_H_

#include <stdint.h>

typedef uintptr_t __mmu_paddr_t;
typedef uintptr_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

/* Section mode */
#define __MMU_PTE_SHIFT	20
#define __MMU_PMD_SHIFT	20
#define __MMU_PGD_SHIFT	20

#endif /* ARM_MMU_H_ */
