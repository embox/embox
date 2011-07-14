/**
 * @file
 *
 * @date 30.06.10
 * @author Anton Kozlov
 */

#include <hal/mm/mmu_core.h>

typedef void (*mmu_page_table_set_t)(mmu_pte_t * ptd, mmu_pte_t * pte);
typedef mmu_pmd_t* (*mmu_page_table_get_t)(mmu_pte_t ptd);

extern unsigned long mmu_page_table_sizes[];

extern unsigned long mmu_table_masks[];

extern unsigned long mmu_level_capacity[];

extern mmu_page_table_set_t mmu_page_table_sets[];

extern mmu_page_table_get_t mmu_page_table_gets[];

