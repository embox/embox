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
#include <prom/prom_printf.h>
#include <util/binalign.h>
#include <math.h>
#include <err.h>

#define MMU_IS_PTD       1
#define MMU_IS_PTE       2
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
		pgd_table[pgd_idx] = ((uint32_t) ptd_table) | MMU_IS_PTD; /* writable and presented */
	} else {
		ptd_table = (mmu_pte_t *) (pgd_table[pgd_idx] & (~((1 << 12) - 1)));
	}

	/* Get pte_table */
	if (0 == (uint32_t) (ptd_table[ptd_idx] & MMU_PAGE_PRESENT)) { /* we have been not allocated page for this region yet */
		pte_table = alloc_pte_table();
		ptd_table[ptd_idx] = ((uint32_t) pte_table) | MMU_IS_PTD; /* writable and presented */
	} else {
		pte_table = (mmu_pte_t *) (ptd_table[ptd_idx] & (~((1 << 12) - 1)));
	}

	pte_table[pte_idx] = (addr & ~MMU_PAGE_MASK);
	pte_table[pte_idx] |= fl | MMU_IS_PTE; /* writable and presented */
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


#if 0
/* FIXME: cleanup function, actually, this is a Spaghetti code */
int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, mmu_page_flags_t flags) {
	uint8_t cur_level;
	uint32_t cur_offset;
	signed long treg_size;
	mmu_pte_t pte;
	paddr_t paddr;
	void *table;
	vaddr_t vaddr = 0;
	context[1] = (mmu_pte_t *) mmu_get_root(ctx); // XXX cast
	/* assuming addresses aligned to page size */
	phy_addr &= ~MMU_PAGE_MASK;
	virt_addr &= ~MMU_PAGE_MASK;
	reg_size &= ~MMU_PAGE_MASK;
	treg_size = reg_size & (~(MMU_PAGE_MASK));
	/* will map the best fitting area on each step
	 * will choose smaller area, while not found the best fitting */
	while (treg_size > 0) {
		for (cur_level = 1; cur_level < 4; cur_level++) {
			cur_offset = ((virt_addr & mmu_table_masks[cur_level])
				>> blog2(mmu_table_masks[cur_level]));
			LOG_DEBUG("level %d; vaddr 0x%8x; paddr 0x%8x; context 0x%8x offset 0x%8x\n",
				cur_level, (uint32_t)virt_addr,
				(uint32_t)phy_addr, context[cur_level], cur_offset);
			vaddr += mmu_level_capacity[cur_level] * cur_offset;
			/* if mapping vaddr is aligned and if required size is pretty fit */
			LOG_DEBUG("%x %x \n", treg_size, mmu_level_capacity[cur_level]);
			if ((virt_addr % mmu_level_capacity[cur_level] == 0) &&
				//(phy_addr % mmu_level_capacity[cur_level] == 0) &&
				(mmu_level_capacity[cur_level] <= treg_size)) {
				LOG_DEBUG("exiting\n");
				break;
			}
			/* if there is mapping allready */
			//TODO untested!
			if (mmu_is_pte(*(context[cur_level] + cur_offset))) {
				LOG_DEBUG("already mapped\n");
				paddr = mmu_pte_extract(*(context[cur_level] + cur_offset));
				*(context[cur_level] + cur_offset) = 0;
				/* need to divide -- old part rests from left */
				if (vaddr < virt_addr && (virt_addr - vaddr > 0)) {
					mmu_map_region(ctx, paddr, vaddr,
						virt_addr - vaddr,
						mmu_flags_extract(*(context[cur_level] + cur_offset)));
				}
				/* old part remains from right */
				else {
					if (mmu_level_capacity[cur_level] - treg_size > 0) {
						mmu_map_region(ctx, paddr + treg_size,
							vaddr + treg_size,
							mmu_level_capacity[cur_level] - treg_size,
							mmu_flags_extract(*(context[cur_level] + cur_offset)));
					}
				}
			}
			/* if there is no next-level page table */
			prom_printf("0x%x, 0x%x\n", *(context[cur_level] + cur_offset),
				mmu_valid_entry(*(context[cur_level] + cur_offset)));
			if (!mmu_valid_entry(*(context[cur_level] + cur_offset))) {
				LOG_DEBUG("no mapping\n");
				/* there is no middle page - creating */
				table = (void *) mmu_table_alloc(mmu_page_table_sizes[cur_level + 1]);
				if (table == NULL) {
					return -1;
				}
				/* setting it's pointer to a prev level page */
				(*mmu_page_table_sets[cur_level])(context[cur_level] + cur_offset,
					(void *) table);
			}
			/* going to the next level map */
			pte = (mmu_pte_t) (context[cur_level] + cur_offset);
			context[cur_level + 1] = (*mmu_page_table_gets[cur_level])(pte);

			LOG_DEBUG("next pte is 0x%8x\n", *((unsigned long *) pte));
			LOG_DEBUG("going down to 0x%8x\n", context[cur_level + 1]);
		}
		/* we are on the best fitting level - creating mapping */
		mmu_set_pte(context[cur_level] + cur_offset, mmu_pte_format(phy_addr, flags));
		LOG_DEBUG("pte is %x\n", mmu_pte_format(phy_addr, flags));
		/* reducing mapping area */
		treg_size -= mmu_level_capacity[cur_level];
		phy_addr += mmu_level_capacity[cur_level];
		virt_addr += mmu_level_capacity[cur_level];
	}
	return 0;
}
#endif
