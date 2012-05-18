/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for X86 architecture.
 *
 * @date 19.03.12
 * @author Gleb Efimov
 */


#include <embox/unit.h>

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_core.h>

#define START 0x5C0000


EMBOX_UNIT_INIT(mmu_init);

void mmu_init(void) {
		uint32_t address = 0x0;
		uint32_t *pdt = (uint32_t *)START;
		uint32_t *pte;

		for (int i = 0; i < MMU_GTABLE_SIZE; i++) {
			pte = pdt + (i + 1) * MMU_PAGE_SIZE;
			pdt[i] = ((uint32_t)&pte) | 3;

			for (int i = 0; i < MMU_MTABLE_SIZE; i++) {
			pte[i] = address | 3;
			address += MMU_PAGE_SIZE;
			}
		}

		pdt[0] = (uint32_t)pte;
		pdt[0] |= 3;

		asm ("mov %0, %%cr3":: "b"(pdt));
}

void mmu_on(void) {

	asm (
		"mov %cr0, %eax\n"
		"or  $0x80000000, %eax\n"
		"mov %eax, %cr0"
	);
}

void mmu_off(void) {
	asm (
		"mov %cr0, %eax\n"
		"and  $0x7fffffff, %eax\n"
		"mov %eax, %cr0"
	);
}

int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, mmu_page_flags_t flags) {
	int* pointer_entry;
	int pde_index = (uint32_t)virt_addr >> 22;
	int pte_index = (uint32_t)virt_addr >> 12 & 0x03ff;

	if (pde_index & 0x1 == 0)

	return -1;
}
