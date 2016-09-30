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

#include <module/embox/arch/mmu.h>

#ifdef __MODULE__embox__arch__arm__mmu_section__H_
/* Section mode */
#define __MMU_PTE_SHIFT	20
#define __MMU_PMD_SHIFT	20
#define __MMU_PGD_SHIFT	20
#elif defined __MODULE__embox__arch__arm__mmu_small_page__H_
/* Small page mode */
#define __MMU_PTE_SHIFT	12
#define __MMU_PMD_SHIFT	12
#define __MMU_PGD_SHIFT	20
#else
#error Unsupported paging mode
#endif

/* Bits 0:1 are for section mode
 * Bits 3:2 are for cache and buffer. They are clear for now. */
#define ARM_MMU_TYPE_PAGE     0x0001
#define ARM_MMU_TYPE_SECTION  0x0002

#define ARM_MMU_LARGE_PAGE    0x0001
#define ARM_MMU_SMALL_PAGE    0x0002

#define ARM_MMU_SECTION_READ_ACC   0x0800
#define ARM_MMU_SECTION_WRITE_ACC  0x0400

#define ARM_MMU_PAGE_READ_ACC      0x0AA0
#define ARM_MMU_PAGE_WRITE_ACC     0x0550

#endif /* ARM_MMU_H_ */
