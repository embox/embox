/**
 * @file
 *
 * @date Jul 25, 2014
 * @author: Anton Bondarev
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include <mem/vmem.h>
#include <hal/mmu.h>

static void print_usage(void) {
	printf("Usage: mmumap [-h][-p pid] [-t] [-v virt_addr] [-o page_options]\n");
}


int main(int argc, char **argv) {
	int opt;
	pid_t pid = 0;

	uint32_t region_options = 0;
	uintptr_t virt_addr;
	uintptr_t table_addr;

	while (-1 != (opt = getopt(argc, argv, "hp:to:v:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'p':
			/* pid if pid = 0 it's the kernel task */
			pid = strtoul(optarg, NULL, 0);
			break;
		case 't':
			table_addr = (uintptr_t)mmu_get_root(pid);
			printf("table address: 0x%" PRIXPTR "\n", table_addr);
			return 0;
		case 'o':
			region_options = strtoul(optarg, NULL, 0);
			break;
		case 'v':
			virt_addr = strtoul(optarg, NULL, 0);
			break;
		}
	}
	printf("options:0x%X\n", region_options);
	printf("virt_addr: 0x%" PRIXPTR "\n", virt_addr);
	return 0;
}
