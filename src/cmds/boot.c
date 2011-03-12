/**
 * @file
 * @brief boot application image from memory.
 *
 * @date 01.06.10
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <hal/interrupt.h>
#include <hal/mm/mmu_core.h>
#include <asm/prom.h>
#include "uimage.h"

EMBOX_CMD(exec);

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
	}
	printf("Image Type: ");
	switch(hdr->ih_type) {
	case IH_TYPE_KERNEL:
		printf("OS Kernel Image\n");
		break;
	default:
		printf("unknown\n");
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
	}
	printf("Image Name: %s\n", hdr->ih_name);
	return 0;
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	char format = 'r';
	unsigned int load_addr, entry_point;
	image_header_t *hdr;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "f:a:h");
		switch(nextOption) {
		case 'f':
			if (1 != sscanf(optarg, "%c", &format)) {
				LOG_ERROR("wrong format %s\n", optarg);
				return -1;
			}
			break;
		case 'a':
			if (sscanf(optarg, "0x%x", &load_addr) < 0) {
				LOG_ERROR("invalid value \"%s\"\n", optarg);
				return -1;
			}
			break;
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	switch (format) {
	case 'u':
		hdr = (image_header_t *) load_addr;
		uimage_info(load_addr);
		entry_point = hdr->ih_ep;
		break;
	case 'r':
		entry_point = LOAD_ADDR;
		break;
	default:
		return -1;
	}

	bootm_linux(load_addr, entry_point);

	return 0;
}
