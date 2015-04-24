/**
 * @file
 *
 * @date Oct 5, 2012
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <sys/mman.h>
#include <mem/phymem.h>
#include <mem/page.h>
#include <util/binalign.h>
#include <kernel/printk.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(phymem_init);

struct page_allocator *__phymem_allocator;

static int phymem_init(void) {
	extern char _ram_base;
	extern char _ram_size;
	extern char _reserve_end;
	char *const phymem_alloc_start = (char *)
		binalign_bound((uintptr_t) &_reserve_end, PAGE_SIZE());
	char *const phymem_alloc_end = (char *)
		binalign_bound((uintptr_t) &_ram_base + (size_t) &_ram_size, PAGE_SIZE());
	const size_t mem_len = phymem_alloc_end - phymem_alloc_start;
	void *va;

	printk("start=%p, end=%p, size=%zu\n", phymem_alloc_start, phymem_alloc_end, mem_len);

	__phymem_allocator = page_allocator_init(phymem_alloc_start, mem_len, PAGE_SIZE());

	va = mmap_device_memory(phymem_alloc_start,
			binalign_bound(sizeof(struct page_allocator) + __phymem_allocator->bitmap_len, PAGE_SIZE()),
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			(uintptr_t) phymem_alloc_start);
	return phymem_alloc_start == va ? 0 : -EIO;
}

void *phymem_alloc(size_t page_number) {
	void *ptr = page_alloc(__phymem_allocator, page_number);
	if (ptr) {
		void *mptr = mmap_device_memory(ptr, page_number * PAGE_SIZE(), PROT_WRITE | PROT_READ,
				MAP_FIXED, (uintptr_t) ptr);
		if (ptr != mptr) {
			munmap(mptr, PAGE_SIZE());
			page_free(__phymem_allocator, ptr, page_number);
			return NULL;
		}
	}
	return ptr;
}

void phymem_free(void *page, size_t page_number) {
	munmap(page, PAGE_SIZE() * page_number);
	page_free(__phymem_allocator, page, page_number);
}
