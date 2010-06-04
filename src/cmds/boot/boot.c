/**
 * @file
 * @brief boot application image from memory.
 *
 * @date 01.06.10
 * @author Nikolay Korotky
 */

#include <shell_command.h>
#include <hal/interrupt.h>
#include <hal/mm/mmu_core.h>
#include <asm/prom.h>
#include "uimage.h"

#define COMMAND_NAME     "boot"
#define COMMAND_DESC_MSG "boot application image from memory"
#define HELP_MSG         "Usage: boot [-f format] [-i] -a addr"

static const char *man_page =
#include "boot_help.inc"
    ;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

#define KERNBASE        0xf0000000  /* First address the kernel will eventually be */
#define LOAD_ADDR       0x4000      /* prom jumps to us here unless this is elf /boot */

/**
 * OpenProm routine.
 */
static void bootm_kernel(unsigned int addr) {
        interrupt_nr_t interrupt_nr;
        extern unsigned int _text_start, _data_start, __stack;
        void (*kernel)(struct linux_romvec *);

        for (interrupt_nr = 0; interrupt_nr < INTERRUPT_NRS_TOTAL; ++interrupt_nr) {
                interrupt_disable(interrupt_nr);
        }

        /* disable mmu */
        mmu_off();
//        __asm__ __volatile__("flush\n\t");
        asm __volatile__ ("set 0x7fffff60, %fp\n"
                          "add %fp, -0x60, %sp\n");
        /* init prom info struct */
        leon_prom_init();
        /* mark as used for bootloader */
        mark();

        mmu_map_region((mmu_ctx_t)0, (uint32_t) &_text_start,
                       (uint32_t) &_text_start, 0x1000000,
                       MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
        mmu_map_region((mmu_ctx_t)0, (uint32_t) 0x44000000,
        		(uint32_t) 0xf4000000, 0x1000000,
        		MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
        mmu_map_region((mmu_ctx_t)0, (uint32_t) 0x7f000000,
                        (uint32_t) 0x7f000000, 0x1000000,
                        MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
        if (&__stack > (&_text_start + 0x1000000)) {
                /* if have to map data sections */
                mmu_map_region((mmu_ctx_t)0, (paddr_t)&_data_start, (vaddr_t)&_data_start,
                               0x1000000, MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
        }
        //TODO mmu fix direct io map
        mmu_map_region((mmu_ctx_t)0, (uint32_t) 0x80000000,
                       (uint32_t) 0x80000000, 0x1000000, MMU_PAGE_WRITEABLE );

        mmu_map_region((mmu_ctx_t)0, (paddr_t)addr, KERNBASE, 0x1000000,
                       MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

        mmu_set_context(0);
//        __asm__ __volatile__("flush\n\t");
        mmu_on();
        mmu_flush_tlb_all();
        /* call kernel */

        kernel = (void (*)(struct linux_romvec*))(KERNBASE + LOAD_ADDR);
        kernel(__va(&spi.romvec));
}

/**
 * Print U-Boot image header.
 */
static int uimage_info(unsigned int addr) {
        image_header_t *hdr = (image_header_t *)addr;
        printf("## Checking uImage at 0x%08X ...\n", addr);
        printf("Image Header Magic Number: 0x%08X\n", hdr->ih_magic);
        printf("Image Header CRC Checksum: 0x%08X\n", hdr->ih_hcrc);
        printf("Image Creation Timestamp: 0x%08X\n", hdr->ih_time);
        printf("Image Data Size: 0x%08X\n", hdr->ih_size);
        printf("Data  Load  Address: 0x%08X\n", hdr->ih_load);
        printf("Entry Point Address: 0x%08X\n", hdr->ih_ep);
        printf("Image Data CRC Checksum: 0x%08X\n", hdr->ih_dcrc);
        printf("Operating System: 0x%08X\n", hdr->ih_os);
        printf("CPU architecture: 0x%08X\n", hdr->ih_arch);
        printf("Image Type: 0x%08X\n", hdr->ih_type);
        printf("Compression Type: 0x%08X\n", hdr->ih_comp);
        printf("Image Name: %s\n", hdr->ih_name);
        return 0;
}

/**
 * Das U-Boot routine.
 */
static void ubootm_kernel(unsigned int addr) {
	void (*kernel) (struct linux_romvec *);
	image_header_t *hdr = (image_header_t *)addr;
	kernel = (void (*)(struct linux_romvec *))hdr->ih_ep;
	//TODO: mmu on and so far
	kernel(__va(&spi.romvec));
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	char format = 'r';
	unsigned int addr;
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
        		if (sscanf(optarg, "0x%x", &addr) < 0) {
            			LOG_ERROR("invalid value \"%s\"\n", optarg);
            			return -1;
        		}
        		break;
    		case 'h':
        		show_help();
        		return 0;
    		case -1:
        		break;
    		default:
        		return 0;
    		}
	} while(-1 != nextOption);

	switch (format) {
	case 'u':
    		uimage_info(addr);
    		ubootm_kernel(addr);
		break;
	case 'r':
    		bootm_kernel(addr);
    		break;
    	default:
                break;
	}
	return 0;
}

