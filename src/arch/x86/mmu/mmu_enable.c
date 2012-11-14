/**
 * @file
 * @brief
 *
 * @date 12.11.12
 * @author Anton Bulychev
 */

#include <types.h>

#include <asm/flags.h>

#define PAGE_4MB         0x400000UL
#define DEFAULT_FLAGS    (MMU_PAGE_PRESENT | MMU_PAGE_WRITABLE \
						 | MMU_PAGE_DISABLE_CACHE | MMU_PAGE_4MB)

static uint32_t boot_page_dir[0x400] __attribute__ ((section(".data"), aligned(0x1000)));

void mmu_enable(void) {
	for (unsigned int i = 0; i < 0x400; i++) {
		boot_page_dir[i] = DEFAULT_FLAGS | (PAGE_4MB * i);
	}

	/*
	 * 1. Setting page directory
	 * 2. Enable 4MB paging.
	 * 3. Enable MMU.
	 */
	__asm__ __volatile__ (
		"mov %0, %%ecx\n\t"  \
		"mov %%ecx, %%cr3\n\t"            \
                                        \
    	"mov %%cr4, %%ecx\n\t"            \
    	"or %1, %%ecx\n\t"      \
    	"mov %%ecx, %%cr4\n\t"            \
                                        \
    	"mov %%cr0, %%ecx\n\t"            \
    	"or %2, %%ecx\n\t"      \
    	"mov %%ecx, %%cr0"
    	:
    	:"r" ((uint32_t) boot_page_dir),
    	 "r" (X86_CR4_PSE),
    	 "r" (X86_CR0_PG)
    );
}
