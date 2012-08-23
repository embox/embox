/**
 * @file
 * @brief Implements mapping interface for X86 architecture.
 *
 * @date 15.03.12
 * @author Gleb Efimov
 */

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_types.h>
#include <util/binalign.h>

static uint32_t pte_table[0x400] __attribute__((aligned(MMU_PAGE_SIZE)));
//todo must be global
#define MMU_PAGE_PRESENT 0x1

static void mmu_map_single_pte(mmu_ctx_t ctx, uint32_t pdt_idx, uint32_t pte_idx, uint32_t addr, mmu_page_flags_t flags) {
	mmu_pgd_t *pdt;
	pdt = mmu_get_root(ctx);

	if (0 == (uint32_t) (pdt[pdt_idx] & MMU_PAGE_PRESENT)) { /* we have been not allocated page for this region yet */
		pdt[pdt_idx] = ((uint32_t) pte_table) | flags | 0x1; /* writable and presented */
	}

	pte_table[pte_idx] = (addr & ~MMU_PAGE_MASK);
	pte_table[pte_idx] |= flags | MMU_PAGE_PRESENT; /* writable and presented */

}

int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, mmu_page_flags_t flags) {
	uint32_t pdt_idx, pte_idx;

	if(0 == (reg_size = __binalign_mask(reg_size, MMU_PAGE_MASK))) {
		return 0;
	}
	virt_addr = __binalign_mask(virt_addr, MMU_PAGE_MASK);
	phy_addr = __binalign_mask(phy_addr, MMU_PAGE_MASK);

	while(reg_size > 0) {
		pdt_idx = (virt_addr & 0xFFC00000) >> 22;
		pte_idx = (virt_addr & 0x003FF000) >> 12;
		mmu_map_single_pte(ctx, pdt_idx, pte_idx, phy_addr, flags);

		virt_addr += MMU_PAGE_SIZE;
		phy_addr += MMU_PAGE_SIZE;
		reg_size -= MMU_PAGE_SIZE;
	}

	return 0;
}

paddr_t mmu_translate(mmu_ctx_t ctx, vaddr_t vaddr) {
	return 0;
}
