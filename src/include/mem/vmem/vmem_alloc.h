/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_ALLOC_H_
#define VMEM_ALLOC_H_

#include <stddef.h>

#include <hal/mmu.h>

extern mmu_pgd_t *vmem_alloc_pgd_table(void);
extern mmu_pmd_t *vmem_alloc_pmd_table(void);
extern mmu_pte_t *vmem_alloc_pte_table(void);
extern void *vmem_alloc_page(void);

extern void vmem_free_pgd_table(mmu_pgd_t *pgd);
extern void vmem_free_pmd_table(mmu_pmd_t *pmd);
extern void vmem_free_pte_table(mmu_pte_t *pte);
extern void vmem_free_page(void *addr);

#endif /* VMEM_ALLOC_H_ */
