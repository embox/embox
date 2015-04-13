/**
 * @file
 *
 * @date Jul 22, 2014
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include <mem/vmem.h>

static void print_usage(void) {
	printf("Usage: mmumap [-h][-p pid][-a phy_addr] [-s size] [-o page_options][-v virt_addr] \n");
}

int mmumap(pid_t pid, uintptr_t virt_addr, uintptr_t phy_addr, size_t region_size, uint32_t region_options) {
	return 0;
}

int main(int argc, char **argv) {
	int opt;
	pid_t pid = 0;
	uintptr_t phy_addr;
	size_t region_size = VMEM_PAGE_SIZE;
	uint32_t region_options = 0;
	uintptr_t virt_addr;

	while (-1 != (opt = getopt(argc, argv, "hp:a:s:o:v:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'p':
			/* pid if pid = 0 it's the kernel task */
			pid = strtoul(optarg, NULL, 0);
			break;
		case 'a':
			/* map address */
			phy_addr = strtoul(optarg, NULL, 0);
			break;
		case 's':
			region_size = strtoul(optarg, NULL, 0);
			break;
		case 'o':
			region_options = strtoul(optarg, NULL, 0);
			break;
		case 'v':
			virt_addr = strtoul(optarg, NULL, 0);
			break;
		}

	}

	mmumap(pid, virt_addr, phy_addr, region_size, region_options);
	return 0;
}
