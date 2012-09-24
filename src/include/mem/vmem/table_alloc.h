/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_TABLE_ALLOC_H_
#define VMEM_TABLE_ALLOC_H_

#include <hal/mm/mmu_types.h>

extern mmu_pte_t *alloc_pte_table(void);

#endif /* VMEM_TABLE_ALLOC_H_ */
