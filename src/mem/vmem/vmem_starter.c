/**
 * @file
 *
 * @date Aug 26, 2014
 * @author: Anton Bondarev
 */

#include <embox/unit.h>
#include <mem/vmem.h>

EMBOX_UNIT_INIT(vmem_starter_init);

static int vmem_starter_init(void) {

	vmem_set_context(1);
	mmu_on();
	return 0;
}
