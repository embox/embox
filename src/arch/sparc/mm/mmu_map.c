/**
 * @file
 *
 * @date 30.06.10
 * @author Anton Kozlov
 */

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/table_alloc.h>
#include <asm/hal/mm/mmu_core.h>
#include <mem/vmem/virtalloc.h>
#include <util/binalign.h>
#include <math.h>


#define MMU_PAGE_PRESENT 3

static void mmu_map_single_pte(mmu_ctx_t ctx, uint32_t pgd_idx, uint32_t ptd_idx, uint32_t pte_idx, uint32_t addr, mmu_page_flags_t flags) {
	mmu_pgd_t *pgd_table;
	mmu_pmd_t *ptd_table;
	mmu_pte_t *pte_table;

	int fl = (0x3) << 2;

	pgd_table = mmu_get_root(ctx);

	/* Get ptd_table */
	if (0 == (uint32_t) (pgd_table[pgd_idx] & MMU_PAGE_PRESENT)) { /* we have been not allocated page for this region yet */
		ptd_table = alloc_pte_table();
		mmu_pgd_set(pgd_table + pgd_idx, ptd_table);
	} else {
		ptd_table = mmu_pgd_get(pgd_table + pgd_idx);
	}

	/* Get pte_table */
	if (0 == (uint32_t) (ptd_table[ptd_idx] & MMU_PAGE_PRESENT)) { /* we have been not allocated page for this region yet */
		pte_table = alloc_pte_table();
		mmu_pmd_set(ptd_table + ptd_idx, pte_table);
	} else {
		pte_table = mmu_pmd_get(ptd_table + ptd_idx);
	}

	pte_table[pte_idx] = mmu_pte_format(addr, fl);
}

int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, mmu_page_flags_t flags) {
	uint32_t pgd_idx, ptd_idx, pte_idx;

	if(0 == (reg_size = __binalign_mask(reg_size, MMU_PAGE_MASK))) {
		return 0;
	}

	virt_addr = __binalign_mask(virt_addr, MMU_PAGE_MASK);
	phy_addr = __binalign_mask(phy_addr, MMU_PAGE_MASK);

	while(reg_size > 0) {
		pgd_idx = (virt_addr & 0xFF000000) >> 24;
		ptd_idx = (virt_addr & 0x00FC0000) >> 18;
		pte_idx = (virt_addr & 0x0003F000) >> 12;

		mmu_map_single_pte(ctx, pgd_idx, ptd_idx, pte_idx, phy_addr, flags);

		virt_addr += MMU_PAGE_SIZE;
		phy_addr += MMU_PAGE_SIZE;
		reg_size -= MMU_PAGE_SIZE;
	}

	return 0;
}
