/**
 * @file
 * @brief boot application image from memory.
 *
 * @date 01.06.10
 * @author Nikolay Korotky
 */

#include <errno.h>
#include <unistd.h>
#include <drivers/irqctrl.h>
#include <hal/mm/mmu_core.h>
#include <stdio.h>
#include "uimage.h"

#define KERNBASE    0xf0000000
#define LOAD_ADDR   (KERNBASE + 0x4000)

static void print_usage(void) {
	printf("Usage: boot [-f format] -a addr\n");
}

/**
 * Print U-Boot image header.
 */
static int uimage_info(unsigned int addr) {
	image_header_t *hdr = (image_header_t *) addr;
	printf("## Checking uImage at 0x%08X ...\n", addr);
	printf("Image Header Magic Number: 0x%08X\n", hdr->ih_magic);
	printf("Image Header CRC Checksum: 0x%08X\n", hdr->ih_hcrc);
	printf("Image Creation Timestamp: %d\n", hdr->ih_time);
	printf("Image Data Size: 0x%08X\n", hdr->ih_size);
	printf("Data  Load  Address: 0x%08X\n", hdr->ih_load);
	printf("Entry Point Address: 0x%08X\n", hdr->ih_ep);
	printf("Image Data CRC Checksum: 0x%08X\n", hdr->ih_dcrc);
	printf("Operating System: ");
	switch(hdr->ih_os) {
	case IH_OS_LINUX:
		printf("linux\n");
		break;
	default:
		printf("unknown\n");
		break;
	}
	printf("CPU architecture: ");
	switch(hdr->ih_arch) {
	case IH_ARCH_SPARC:
		printf("sparc\n");
		break;
	case IH_ARCH_MICROBLAZE:
		printf("microblaze\n");
		break;
	default:
		printf("unknown\n");
		break;
	}
	printf("Image Type: ");
	switch(hdr->ih_type) {
	case IH_TYPE_KERNEL:
		printf("OS Kernel Image\n");
		break;
	default:
		printf("unknown\n");
		break;
	}
	printf("Compression Type: ");
	switch(hdr->ih_comp) {
	case IH_COMP_NONE:
		printf("none\n");
		break;
	case IH_COMP_GZIP:
		printf("gzip\n");
		break;
	case IH_COMP_BZIP2:
		printf("bzip2\n");
		break;
	case IH_COMP_LZMA:
		printf("lzma\n");
		break;
	default:
		printf("unknown\n");
		break;
	}
	printf("Image Name: %s\n", hdr->ih_name);
	return 0;
}

int main(int argc, char **argv) {
	int opt;
	char format = 'r';
	unsigned int load_addr;
	void (*entry_point)(void);
	image_header_t *hdr;
	while (-1 != (opt = getopt(argc, argv, "f:a:h"))) {
		switch(opt) {
		case 'f':
			if (1 != sscanf(optarg, "%c", &format)) {
				printf("wrong format %s\n", optarg);
				return -EINVAL;
			}
			break;
		case 'a':
			if (sscanf(optarg, "0x%x", &load_addr) < 0) {
				printf("invalid value \"%s\"\n", optarg);
				return -EINVAL;
			}
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	switch (format) {
	case 'u':
		hdr = (image_header_t *) load_addr;
		uimage_info(load_addr);
		entry_point = (void (*)(void)) hdr->ih_ep;
		break;
	case 'r':
		entry_point = (void (*)(void)) LOAD_ADDR;
		break;
	default:
		return -EINVAL;
	}

#if 0
	bootm_linux(load_addr, entry_point);
#else
	entry_point();
#endif

	return 0;
}
