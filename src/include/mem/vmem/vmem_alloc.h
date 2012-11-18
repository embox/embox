/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_ALLOC_H_
#define VMEM_ALLOC_H_

#include <hal/mmu.h>

extern void *VIRTUAL_TABLES_START;
extern size_t VIRTUAL_TABLES_LEN;

extern void *VIRTUAL_PAGES_INFO_START;
extern size_t VIRTUAL_PAGES_INFO_LEN;

extern mmu_pgd_t *vmem_alloc_pgd_table(void);
extern mmu_pmd_t *vmem_alloc_pmd_table(void);
extern mmu_pte_t *vmem_alloc_pte_table(void);
extern void *vmem_alloc_page(void);

#endif /* VMEM_ALLOC_H_ */
