/**
 * @file
 *
 * @date Oct 5, 2012
 * @author: Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>
#include <sys/mman.h>
#include <mem/phymem.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(phymem_init);

struct page_allocator *__phymem_allocator;

extern char _ram_base;
extern char _ram_size;

static int phymem_init(void) {
	char *const phymem_alloc_start = phymem_allocated_start();
	char *const phymem_alloc_end = phymem_allocated_end();
	const size_t mem_len = phymem_alloc_end - phymem_alloc_start;
	void *va;

	if (phymem_alloc_start > phymem_alloc_end) {
		log_error("phymem_start (%p) > phymem_end (%p)",
		           phymem_alloc_start, phymem_alloc_end);

		return -1;
	}

	log_info("start=%p end=%p size=%zu\n", phymem_alloc_start, phymem_alloc_end, mem_len);

	va = mmap_device_memory(phymem_alloc_start,
			mem_len,
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			(uint64_t)((uintptr_t) phymem_alloc_start));

	if (va) {
		__phymem_allocator = page_allocator_init(phymem_alloc_start, mem_len, PAGE_SIZE());
	}
	return phymem_alloc_start == va ? 0 : -EIO;
}

/* Find section within RAM. */
static uintptr_t find_sections_end(void *vmas[], unsigned int lens[]) {
	int i = 0;
	uintptr_t l = 0, end, ram_end;

	ram_end = (uintptr_t) &_ram_base + (size_t) &_ram_size;

	while (vmas[i]) {
		if (lens[i]) {
			end = (uintptr_t) vmas[i] + lens[i];

			if (end <= ram_end && end > l) {
				l = (uintptr_t) vmas[i] + lens[i];
			}
		}
		i++;
	}

	return l;
}

char *phymem_allocated_start(void) {
	extern char _reserve_end;

	extern void *sections_text_vma[];
	extern unsigned int sections_text_len[];
	extern void *sections_data_vma[];
	extern unsigned int sections_data_len[];
	extern void *sections_rodata_vma[];
	extern unsigned int sections_rodata_len[];
	extern void *sections_bss_vma[];
	extern unsigned int sections_bss_len[];
	struct {
		void **vmas;
		unsigned int *lens;
	} sect[4] = {
		{ sections_text_vma, sections_text_len },
		{ sections_rodata_vma, sections_rodata_len },
		{ sections_data_vma, sections_data_len },
		{ sections_bss_vma, sections_bss_len },
	};

	uintptr_t sections_end, tmp;
	int i;

	sections_end = (uintptr_t) &_reserve_end;

	for (i = 0; i < 4; i++) {
		tmp = find_sections_end(sect[i].vmas, sect[i].lens);
		if (sections_end < tmp) {
			sections_end = tmp;
		}
	}

	return (char *) binalign_bound(sections_end, PAGE_SIZE());
}

char *phymem_allocated_end(void) {
	return (char *)
			binalign_bound((uintptr_t) &_ram_base + (size_t) &_ram_size, PAGE_SIZE());
}

void *phymem_alloc(size_t page_number) {
	return page_alloc(__phymem_allocator, page_number);
}

void phymem_free(void *page, size_t page_number) {
	page_free(__phymem_allocator, page, page_number);
}

struct page_allocator *phy_allocator(void) {
	return __phymem_allocator;
}
