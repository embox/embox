/**
 * @file
 * @brief
 *
 * @author  Evgenij Bogdanov
 * @date    09.07.2018
 */

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

#include <mem/phymem.h>
#include <mem/vmem/vmem_alloc.h>
#include <mem/vmem.h>
#include <drivers/common/memory.h>

static void print_vmem(struct page_allocator *pgd_allocator,
		struct page_allocator *pmd_allocator,
		struct page_allocator *pte_allocator) {

	printf("\nVIRTUAL MEMORY:\n");

	printf("PGD tables count / free - %zu / %zu\n", pgd_allocator->pages_n,
				pgd_allocator->free / pgd_allocator->page_size);
	printf("PMD tables count / free - %zu / %zu\n", pmd_allocator->pages_n,
				pmd_allocator->free / pmd_allocator->page_size);
	if (pte_allocator) {
		printf("PTE tables count / free - %zu / %zu\n", pte_allocator->pages_n,
				pte_allocator->free / pte_allocator->page_size);
	} else {
		printf("PTE do not available in this configuration\n");
	}

	printf("-----------------------------------------\n");
}

static void print_phymem(struct page_allocator *allocator) {
	char *const phymem_alloc_start = phymem_allocated_start();
	char *const phymem_alloc_end = phymem_allocated_end();
	const size_t mem_len = phymem_alloc_end - phymem_alloc_start;

	printf("\nPHYSICAL MEMORY:\n");
	printf("start=%p end=%p size=0x%x\n",
			phymem_alloc_start, phymem_alloc_end, mem_len);
	printf("-----------------pages-------------------\n");
	printf("first usable phy_page adr - %p\n", allocator->pages_start);
	printf("pages count / free - %zu / %zu\n", allocator->pages_n,
				allocator->free / allocator->page_size);
	printf("-----------------------------------------\n");
}

static void print_sections(void) {
	extern char _text_vma, _rodata_vma, _data_vma, _bss_vma;
	extern char _text_len, _rodata_len, _data_len, _bss_len, _bss_len_with_reserve;
	extern char _bss_end, _reserve_end;

	printf("\nSECTIONS:\n");
	printf("TEXT:   begin - %p, end - %p, len - %p\n",
			&_text_vma, (int) &_text_vma + &_text_len, &_text_len);
	printf("RODATA: begin - %p, end - %p, len - %p\n",
			&_rodata_vma, (int) &_rodata_vma + &_rodata_len, &_rodata_len);
	printf("DATA:   begin - %p, end - %p, len - %p\n",
			&_data_vma, (int) &_data_vma + &_data_len, &_data_len);
	printf("BSS:    begin - %p, end - %p, len - %p\n        "
			"reserve - %p, end_with_reserve - %p, len_with_reserve - %p\n",
			&_bss_vma, &_bss_end, &_bss_len,
			-(int) &_bss_end + &_reserve_end,
			&_reserve_end, &_bss_len_with_reserve);

	printf("-----------------------------------------\n\n");
}

static void print_segments(void) {
	struct periph_memory_desc *buff[PERIPH_MAX_SEGMENTS];
	int size = periph_desc(buff);

	printf("\nPERIPH SEGMENTS:\n");

	for (int i = 0; i < size; i++) {
		printf("seg_num %d: start - 0x%" PRIx32 ", end - 0x%" PRIx32 ", len - 0x%" PRIx32 "\n",
				i, buff[i]->start, buff[i]->start + buff[i]->len, buff[i]->len);
	}

	printf("-----------------------------------------\n");
}

static void print_usage(void) {
	printf("Usage: infomem [-hpsvi]\n");
}

static void show_all(void) {
	print_phymem(phy_allocator());
	print_vmem(get_pgd_allocator(), get_pmd_allocator(), get_pte_allocator());
	print_segments();
	print_sections();
}

int main(int argc, char **argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "hpsvi"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'p':
			print_phymem(phy_allocator());
			return 0;
		case 's':
			print_sections();
			return 0;
		case 'v':
			print_vmem(get_pgd_allocator(), get_pmd_allocator(), get_pte_allocator());
			return 0;
		case 'i':
			print_segments();
			return 0;
		default:
			printf("unknown argument!\n");
			return -1;
		}
	}

	show_all();
	return 0;
}
