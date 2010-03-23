/**
 * @file
 *
 * @date 09.03.2010
 * @author Anton Bondarev
 */

#include <types.h>

#include <embox/test.h>
#include <traps_core.h>
#include <hal/mm/mmu_core.h>

EMBOX_TEST(run);



static int run() {
	mmu_staus_t prev_mmu_status;
	mmu_table_t prev_mmu_table;

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
