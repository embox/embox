/**
 * @file
 * @brief Specifies MMU interface.
 *
 * @date 10.03.10
 * @author Anton Bondarev
 */

#ifndef HAL_MMU_H_
#define HAL_MMU_H_

#include <module/embox/arch/mmu.h>

typedef __mmu_paddr_t mmu_paddr_t;
typedef __mmu_vaddr_t mmu_vaddr_t;

typedef __mmu_page_flags_t mmu_page_flags_t;

typedef __mmu_pgd_t mmu_pgd_t;
typedef __mmu_pmd_t mmu_pmd_t;
typedef __mmu_pte_t mmu_pte_t;

typedef __mmu_ctx_t mmu_ctx_t;

#define MMU_PTE_SHIFT     __MMU_PTE_SHIFT
#define MMU_PMD_SHIFT     __MMU_PMD_SHIFT
#define MMU_PGD_SHIFT     __MMU_PGD_SHIFT

#define MMU_PTE_ENTRIES   (1UL << (MMU_PMD_SHIFT - MMU_PTE_SHIFT))
#define MMU_PMD_ENTRIES   (1UL << (MMU_PGD_SHIFT - MMU_PMD_SHIFT))
#define MMU_PGD_ENTRIES   (1UL << (sizeof(mmu_vaddr_t) * 8 - MMU_PGD_SHIFT))

#define MMU_PTE_MASK      ((MMU_PTE_ENTRIES - 1) << MMU_PTE_SHIFT)
#define MMU_PMD_MASK      ((MMU_PMD_ENTRIES - 1) << MMU_PMD_SHIFT)
#define MMU_PGD_MASK      ((MMU_PGD_ENTRIES - 1) << MMU_PGD_SHIFT)

#if 0
#define MMU_PGD_SIZE      (1UL << MMU_PGD_SHIFT)
#define MMU_PMD_SIZE      (1UL << MMU_PMD_SHIFT)
#define MMU_PTE_SIZE      (1UL << MMU_PTD_SHIFT)
#endif


#endif /* HAL_MMU_H_ */
