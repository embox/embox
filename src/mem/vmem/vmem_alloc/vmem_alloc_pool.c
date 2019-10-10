/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <string.h>

#include <mem/page.h>

#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>

#include <framework/mod/options.h>

static char lvl0_raw[(1024) * MMU_SIZE(0)] __attribute__ ((aligned(MMU_SIZE(0))));
static char lvl1_raw[(1024) * MMU_SIZE(1)] __attribute__ ((aligned(MMU_SIZE(1))));

PAGE_ALLOCATOR_DEF(lvl0_allocator, lvl0_raw, 1024, MMU_SIZE(0));
PAGE_ALLOCATOR_DEF(lvl1_allocator, lvl1_raw, 1024, MMU_SIZE(1));

uintptr_t *vmem_alloc_table(int lvl) {
	void *addr = 0;

	switch (lvl) {
	case 0:
		addr = page_alloc_zero(&lvl0_allocator, 1);
		break;
	case 1:
		addr = page_alloc_zero(&lvl1_allocator, 1);
		break;
	default:
		log_error("alloc vmem_table lvl (%d) don't support now", lvl);
	}

	return addr;
}

void vmem_free_table(int lvl, uintptr_t *table) {
	switch (lvl) {
	case 0:
		page_free(&lvl0_allocator, table, 1);
		break;
	case 1:
		page_free(&lvl1_allocator, table, 1);
		break;
	default:
		log_error("free vmem_table lvl (%d) don't support now", lvl);
	}
}
