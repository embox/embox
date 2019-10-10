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

#include <mem/phymem.h>
#include <mem/page.h>

#include <mem/vmem/vmem_alloc.h>

uintptr_t *vmem_alloc_table(int lvl) {
	uintptr_t *table;

	table = phymem_alloc(1);
	if (table) {
		memset(table, 0, PAGE_SIZE());
	}

	return table;
}

void vmem_free_table(int lvl, uintptr_t *table) {
	(void )lvl;
	phymem_free(table, 1);
}
