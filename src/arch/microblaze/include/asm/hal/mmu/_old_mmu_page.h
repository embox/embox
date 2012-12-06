/**
 * @file
 *
 * @date 07.04.10
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_MMU_PAGE_H_
#define MICROBLAZE_MMU_PAGE_H_

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_types.h>

static inline size_t __invert_reg_size_convert(uint32_t size) {
	switch (size) {
	case RTLBHI_SIZE_1K: /* 1kb */
		return 0x400;
	case RTLBHI_SIZE_4K: /* 4k field */
		return 0x1000;
	case RTLBHI_SIZE_16K: /* 16k field */
		return 0x4000;
	case RTLBHI_SIZE_64K: /* 64k field */
		return 0x10000;
	case RTLBHI_SIZE_256K: /* 256 kb*/
		return 0x40000;
	case RTLBHI_SIZE_1M: /* 1M field */
		return 0x100000;
	case RTLBHI_SIZE_4M: /* 4M field*/
		return 0x400000;
	case RTLBHI_SIZE_16M: /* 16M field */
		return 0x1000000;
	default: /* wrong size*/
		return -1;
	}
}
int32_t __mmu_find_index(mmu_ctx_t ctx, vaddr_t vaddr) {
	extern void get_utlb_record(int tlbx, uint32_t *tlblo, uint32_t *tlbhi);
	int32_t index;
	uint32_t tlbhi, tlblo, size, mask;
	for (index = 0; index < UTLB_QUANTITY_RECORDS; index++) {
		get_utlb_record(index, &tlblo, &tlbhi);
		size = (tlbhi >> 7) & 0x07;
		mask = ~(__invert_reg_size_convert(size) - 1);

		if ((tlbhi & mask) == (vaddr & mask)) {
			break;
		}
	}
	return index;
}

static inline void __set_mark(mmu_ctx_t ctx, vaddr_t vaddr, uint32_t clear_lomark,
		uint32_t set_lomark, uint32_t clear_himark, uint32_t set_himark) {
	extern void get_utlb_record(int tlbx, uint32_t *tlblo, uint32_t *tlbhi);
	extern void set_utlb_record(int tlbx, uint32_t tlblo, uint32_t tlbhi);

	int32_t index;
	uint32_t tlbhi, tlblo;

	index = __mmu_find_index(ctx, vaddr);

	if (index == UTLB_QUANTITY_RECORDS) {
		return;
	}

	get_utlb_record(index, &tlblo, &tlbhi);

	tlbhi &= ~clear_himark;
	tlblo &= ~clear_lomark;

	tlbhi |= set_himark;
	tlblo |= set_lomark;

	if (1) { //ctx == cur_ctx
		set_utlb_record(index, tlblo, tlbhi);
	}

}

static inline void mmu_page_mark_writable(mmu_ctx_t ctx, vaddr_t vaddr) {
	__set_mark(ctx, vaddr, 0, RTLBLO_WR_BIT, 0, 0);
}

static inline void mmu_page_mark_executeable(mmu_ctx_t ctx, vaddr_t vaddr) {
	__set_mark(ctx, vaddr, 0, RTLBLO_EX_BIT, 0, 0);
}

static inline void mmu_page_mark_cacheable(mmu_ctx_t ctx, vaddr_t vaddr) {
	__set_mark(ctx, vaddr, 0, RTLBLO_WR_BIT, 0, 0);
}

static inline mmu_page_flags_t mmu_page_get_flags(mmu_ctx_t ctx, vaddr_t vaddr) {
	extern mmu_env_t *cur_env;

	__mmu_table_record_t *rec = &cur_env->utlb_table[__mmu_find_index(ctx, vaddr)];
	return (mmu_page_flags_t) rec->tlblo;
}

static inline void mmu_page_set_flags(mmu_ctx_t ctx, vaddr_t vaddr,
						mmu_page_flags_t flags) {
	__set_mark(ctx, vaddr, MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE, flags, 0, 0);
}

static inline void mmu_page_mark_valid(mmu_ctx_t ctx, vaddr_t vaddr) {
	__set_mark(ctx, vaddr, 0, 0, 0, RTLBHI_V_BIT);
}

static inline void mmu_page_mark_invalide(mmu_ctx_t ctx, vaddr_t vaddr) {
	__set_mark(ctx, vaddr, 0, 0, RTLBHI_V_BIT, 0);
}

#endif /* MICROBLAZE_MMU_PAGE_H_ */
