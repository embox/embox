/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for X86 architecture.
 *
 * @date 19.03.12
 * @author Gleb Efimov
 */

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_core.h>

int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, mmu_page_flags_t flags) {
	return 0;
}

void mmu_on(void) {

	asm (
		"mov %cr0, %eax\n"
		"or  $0x80000001, %eax\n"
		"mov %eax, %cr0"
	);
}

void mmu_off(void) {
	asm (
		"mov %cr0, %eax\n"
		"and  $0x7ffffffe, %eax\n"
		"mov %eax, %cr0"
	);
}
