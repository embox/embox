/**
 * @file
 *
 * @data 10.03.2010
 * @author Anton Bondarev
 */

#ifndef MMU_CORE_H_
#define MMU_CORE_H_

void mmu_save_status(uint32_t *status);

void mmu_restore_status(uint32_t *status);

void mmu_save_table(uint32_t *status);

void mmu_restore_table(uint32_t *status);

int mmu_enable(uint32_t table);

#endif /* MMU_CORE_H_ */
