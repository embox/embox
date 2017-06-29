/**
 * @file
 *
 * @date Oct 8, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <stdio.h>
#include <mem/phymem.h>
#include <stdint.h>

static void print_usage(void) {
	printf("Usage: memmap [-hra]\n");
}

extern char _ram_base;
extern char _ram_size;

static void show_regions(void) {
	printf("| region name |   start    |    end     |    size    |    free    |\n");
	printf("|  sdram      | 0x%8X | 0x%8X | 0x%8X | 0x%8X |\n",
			(uint32_t)&_ram_base,
			(uint32_t)&_ram_base + (uint32_t)&_ram_size,
			(uint32_t)__phymem_end, (uint32_t)__phymem_allocator->free);
#if 0
	phymem_alloc(0x10);
	printf("|  sdram      | 0x%8X | 0x%8X | 0x%8X | 0x%8X |\n",
			(uint32_t)&_ram_base,
			(uint32_t)&_ram_base + (uint32_t)&_ram_size,
			(uint32_t)__phymem_end, __phymem_allocator->free);
#endif
}

static void show_all(void) {

}

int main(int argc, char **argv) {
	int opt;
	getopt_init();
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
			show_regions();
			return 0;
		}
	}
	return 0;
}
