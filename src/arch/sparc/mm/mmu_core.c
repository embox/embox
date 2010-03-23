/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */
#include <types.h>
#include <hal/mm/mmu_core.h>
#include <asm/asi.h>

void mmu_save_status(uint32_t *status) {

}

void mmu_restore_status(uint32_t *status) {

}

void mmu_save_table(uint32_t *status) {

}

void mmu_restore_table(uint32_t *status) {

}

int mmu_enable(uint32_t *table) {
	srmmu_set_mmureg(0x00000001);
	return 0;
}

