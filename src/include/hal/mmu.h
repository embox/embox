/**
 * @file
 * @brief
 *
 * @date 04.10.2012
 * @author Anton Bulychev
 */

#ifndef HAL_MMU_H_
#define HAL_MMU_H_

#include <module/embox/arch/mmu.h>

typedef __mmu_paddr_t mmu_paddr_t;
typedef __mmu_vaddr_t mmu_vaddr_t;

typedef __mmu_ctx_t mmu_ctx_t;

typedef __mmu_pgd_t mmu_pgd_t;
typedef __mmu_pmd_t mmu_pmd_t;
typedef __mmu_pte_t mmu_pte_t;

typedef __mmu_page_flags_t mmu_page_flags_t;

#define MMU_PTE_SHIFT     __MMU_PTE_SHIFT
#define MMU_PMD_SHIFT     __MMU_PMD_SHIFT
#define MMU_PGD_SHIFT     __MMU_PGD_SHIFT

#define MMU_PTE_ENTRIES   (1UL << (MMU_PMD_SHIFT - MMU_PTE_SHIFT))
#define MMU_PMD_ENTRIES   (1UL << (MMU_PGD_SHIFT - MMU_PMD_SHIFT))
#define MMU_PGD_ENTRIES   (1UL << (sizeof(mmu_vaddr_t) * 8 - MMU_PGD_SHIFT))

#define MMU_PTE_MASK      ((MMU_PTE_ENTRIES - 1) << MMU_PTE_SHIFT)
#define MMU_PMD_MASK      ((MMU_PMD_ENTRIES - 1) << MMU_PMD_SHIFT)
#define MMU_PGD_MASK      ((MMU_PGD_ENTRIES - 1) << MMU_PGD_SHIFT)

#define MMU_PAGE_SIZE     (1UL << MMU_PTE_SHIFT)
#define MMU_PAGE_MASK     (MMU_PAGE_SIZE - 1)

#define MMU_PGD_SIZE      (1UL << MMU_PGD_SHIFT)
#define MMU_PMD_SIZE      (1UL << MMU_PMD_SHIFT)
#define MMU_PTE_SIZE      (1UL << MMU_PTD_SHIFT)

#define MMU_PAGE_WRITABLE       __MMU_PAGE_WRITABLE
#define MMU_PAGE_USER_MODE      __MMU_PAGE_USER_MODE
#define MMU_PAGE_CACHABLE       __MMU_PAGE_CACHABLE

extern void mmu_on(void);
extern void mmu_off(void);

extern void switch_mm(mmu_ctx_t prev, mmu_ctx_t next);

extern mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd);
extern mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx);

extern mmu_pmd_t *mmu_pgd_follow(mmu_pgd_t *pgd);
extern mmu_pte_t *mmu_pmd_follow(mmu_pmd_t *pmd);
extern mmu_paddr_t mmu_pte_follow(mmu_pte_t *pte);

extern void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd);
extern void mmu_pmd_set(mmu_pgd_t *pmd, mmu_pmd_t *pte);
extern void mmu_pte_set(mmu_pgd_t *pte, mmu_paddr_t addr, mmu_page_flags_t flags);

extern int mmu_pgd_present(mmu_pgd_t *pgd);
extern int mmu_pmd_present(mmu_pmd_t *pmd);
extern int mmu_pte_present(mmu_pte_t *pte);

#endif /* HAL_MMU_H_ */
