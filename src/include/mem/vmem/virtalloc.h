/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_VIRT_ALLOC_H_
#define VMEM_VIRT_ALLOC_H_

#include <hal/mm/mmu_types.h>

extern mmu_pmd_t *alloc_pmd_table(void);
extern mmu_pte_t *alloc_pte_table(void);
extern void *alloc_virt_memory(size_t size);

#endif /* VMEM_VIRT_ALLOC_H_ */
