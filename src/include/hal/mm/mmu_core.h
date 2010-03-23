/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#ifndef HAL_MMU_CORE_H_
#define HAL_MMU_CORE_H_

#include <asm/mmu_core.h>

void mmu_save_status(uint32_t *status);

void mmu_restore_status(uint32_t *status);

void mmu_save_table(uint32_t *status);

void mmu_restore_table(uint32_t *status);

int mmu_enable(uint32_t *table);

#endif /* HAL_MMU_CORE_H_ */
