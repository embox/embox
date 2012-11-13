/**
 * @file
 * @brief
 *
 * @date 12.11.12
 * @author Anton Bulychev
 */

#include <types.h>

#define PAGE_4MB 0x400000UL

static uint32_t boot_page_dir[0x400] __attribute__ ((section(".data"), aligned(0x1000)));

void mmu_enable(void) {
	for (unsigned int i = 0; i < 0x400; i++) {
		boot_page_dir[i] = 0x00000083 | (PAGE_4MB * i);
	}

	/*
	 * 1. Setting page directory
	 * 2. Enable 4MB paging.
	 * 3. Enable MMU.
	 */
	__asm__ volatile (
		"mov $boot_page_dir, %ecx\n\t"  \
		"mov %ecx, %cr3\n\t"            \
                                        \
    	"mov %cr4, %ecx\n\t"            \
    	"or $0x00000010, %ecx\n\t"      \
    	"mov %ecx, %cr4\n\t"            \
                                        \
    	"mov %cr0, %ecx\n\t"            \
    	"or $0x80000000, %ecx\n\t"      \
    	"mov %ecx, %cr0"
    );
}
