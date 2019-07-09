/**
 * @file
 *
 * @date Oct 8, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <util/dlist.h>

#include <mem/phymem.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>
#include <mem/vmem.h>

static void print_usage(void) {
	printf("Usage: memmap [-hra]\n");
}

extern char _ram_base;
extern char _ram_size;

static void show_regions(void) {
	printf("| region name |   start    |    end     |    size    |    free    |\n");
	printf("|  sdram      | 0x%8" PRIxPTR " | 0x%8" PRIxPTR " | 0x%8" PRIxPTR " | 0x%8" PRIxPTR " |\n",
			(uintptr_t)&_ram_base,
			(uintptr_t)&_ram_base + (uintptr_t)&_ram_size,
			(uintptr_t)__phymem_end, (uintptr_t)__phymem_allocator->free);
}

static void show_all(void) {

}

static void show_vmem_translation(void) {
	struct marea *marea;
	struct emmap *emmap;
	uintptr_t voff;
	uintptr_t paddr;
	struct mmu_translate_info mmu_translate_info;

	emmap = task_resource_mmap(task_kernel_task());

	dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
		printf("map region (base 0x%" PRIxPTR " size %zu flags 0x%" PRIx32 ")\n",
				marea->start, marea->size, marea->flags);
		for (voff = 0; voff < marea->size; voff += MMU_PAGE_SIZE) {
			paddr = vmem_translate(emmap->ctx, marea->start + voff, &mmu_translate_info);
			printf("0x%16x -> 0x%16x \n", marea->start + voff, paddr);
		}
	}
}

int main(int argc, char **argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "hra"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'r':
			show_regions();
			return 0;
		case 'a':
			show_all();
			return 0;
		default:
			show_vmem_translation();
			return 0;
		}
	}
	show_vmem_translation();
	return 0;
}
