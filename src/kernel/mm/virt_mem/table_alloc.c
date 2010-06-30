#include <kernel/mm/virt_mem/table_alloc.h>
#include <kernel/mm/virt_mem/mmu_core.h>
#include <lib/page_alloc.h>
#include <hal/mm/mmu_core.h>

typedef struct {
	uint32_t free;
} page_header_t;
uint8_t *cur_page = NULL;
size_t cur_rest = 0;

static pmark_t *clear_page_alloc(void) {
	uint32_t i;
	pmark_t *t = page_alloc();
	for (i = 0; i < PAGE_SIZE >> 2; i++) {
		*(((uint32_t *) t) + i) = 0;
	}
	return t;
}

unsigned long *mmu_table_alloc(size_t size) {
	uint8_t *t, *page;
	if (cur_rest < size) {
		cur_page = (uint8_t *) clear_page_alloc();
#ifdef DEBUG
		printf("---requesting new page %x\n",cur_page);
#endif
		cur_rest = PAGE_SIZE - PAGE_HEADER_SIZE;
		((page_header_t *) cur_page)->free = cur_rest;
		cur_page += PAGE_HEADER_SIZE;
	}
	t = cur_page;
	cur_page += size;
	/* The page table pointed to by a PTP must be
	* aligned on a boundary equal to the size of the page table.
	*/
	cur_rest -= size;
	page = (uint8_t *) (((unsigned long) cur_page) & ~MMU_PAGE_MASK);
	if (cur_rest == 0) {
		page -= PAGE_SIZE;
	}
#ifdef DEBUG
	printf("page %x; page->free %x; cur_rest %x; size %x, return %x\n", page, ((page_header_t *) page)->free, cur_rest, size, t);
#endif
	((page_header_t *) page)->free -= size;
	return (pmd_t *) t;
}

void mmu_table_free(unsigned long *table, int level) {
	uint8_t *page = (uint8_t *) (((unsigned long) table) & ~MMU_PAGE_MASK);
	int i;
	int size = mmu_page_table_sizes[level];
#ifdef DEBUG
	printf("table %x; level %x; page %x; size %x; is free %x\n", (unsigned long) table, level,
			(unsigned long) page, size, ((page_header_t *) page)->free + size);
#endif
	for (i = 0; i < mmu_page_table_sizes[level-1]; i++ ) {
		unsigned long t = *(table + i);
		if (!mmu_is_pte(t)) {
#ifdef DEBUG
			printf("on %x to %x\n", table + i, (t & MMU_PTD_PMASK) << 4);
#endif
			mmu_table_free((*mmu_page_table_gets[level])(t), level + 1);
		}
	}
	((page_header_t *) page)->free += size;
	if (((page_header_t *) page)->free == PAGE_SIZE - PAGE_HEADER_SIZE && page != cur_page) {
		page_free((pmark_t *)page);
	}
}

