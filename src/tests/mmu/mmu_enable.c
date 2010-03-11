/**
 * @file
 *
 * @date 09.03.2010
 * @author Anton Bondarev
 */

#include <types.h>

#include <embox/test.h>
#include <hal/mm/mmu_core.h>

EMBOX_TEST(run);

static uint32_t prev_mmu_status;
static uint32_t prev_mmu_table;

static int run() {
	mmu_save_status(&prev_mmu_status);
	if (prev_mmu_status) {
		mmu_save_table(&prev_mmu_table);
	}
	mmu_enable(prev_mmu_table);

	if(prev_mmu_status) {
		mmu_restore_table(&prev_mmu_table);
	}
	mmu_restore_status(&prev_mmu_status);
	return 0;
}
