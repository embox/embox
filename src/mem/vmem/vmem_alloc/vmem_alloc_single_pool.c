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

#include <hal/mmu.h>
#include <mem/page.h>

#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>

#include <framework/mod/options.h>

#define MMU_TABLES_QUANTITY OPTION_GET(NUMBER,mmu_tables_max)

static char mmu_tables_raw[MMU_PAGE_SIZE][MMU_TABLES_QUANTITY] __attribute__ ((aligned(MMU_PAGE_SIZE)));

PAGE_ALLOCATOR_DEF(mmu_tables_allocator, mmu_tables_raw, MMU_TABLES_QUANTITY, MMU_PAGE_SIZE);

uintptr_t *vmem_alloc_table(int lvl) {
	return page_alloc_zero(&mmu_tables_allocator, 1);
}

void vmem_free_table(int lvl, uintptr_t *table) {
	page_free(&mmu_tables_allocator, table, 1);
}
