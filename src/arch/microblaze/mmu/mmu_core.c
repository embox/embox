/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for Microblaze architecture.
 *
 * @date 10.03.10
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <string.h>
#include <asm/msr.h>
#include <asm/hal/mmu/mmu_core.h>
#include <asm/cache.h>
#include <embox/unit.h>
#include <hal/ipl.h>

#include <hal/mmu.h>
#include <mem/vmem.h>

#include <asm/hal/env/traps_core.h>

#include <kernel/printk.h>

static inline int mmu_map_region(mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr,
		size_t reg_size, uint32_t flags);

void mmu_off(void);

/* Current index in utlb */
static uint32_t cur_utlb_idx = 0;

/* Current virtual context */
static mmu_ctx_t cur_ctx;

static inline mmu_vaddr_t mmu_fault_address(void) {
	uint32_t ret;
	__asm__ __volatile__ (
		"mfs	%0, rear; \n\t"
		: "=r" (ret)
	);
	return (mmu_vaddr_t) ret;
}

static int tlb_miss(int number) {
	mmu_vaddr_t vaddr = mmu_fault_address();
	mmu_paddr_t paddr = vmem_translate(cur_ctx, vaddr, NULL);

	if (paddr) {
		vaddr -= vaddr & MMU_PAGE_MASK;
		paddr -= paddr & MMU_PAGE_MASK;

		mmu_map_region(paddr, vaddr, MMU_PAGE_SIZE, PAGE_WRITEABLE | PAGE_EXECUTEABLE);
	} else {
		printk("Page fault: 0x%08x\n", (unsigned int) vaddr);
	}

	return 1;
}

void mmu_on(void) {
	/* Set tlb miss handler */
	hwtrap_handler[17] = (__trap_handler) tlb_miss;
	hwtrap_handler[18] = (__trap_handler) tlb_miss;
	hwtrap_handler[19] = (__trap_handler) tlb_miss;

	cache_disable();

	__asm__ __volatile__ (
			"msrset  r0, %0;\n\t"
			:
			: "i"(MSR_VM_MASK)
	);
}

void mmu_off(void) {
	__asm__ __volatile__ (
		"msrclr  r0, %0;\n\t"
		:
		: "i"(MSR_VM_MASK)

	);
}

void set_utlb_record(int tlbx, uint32_t tlblo, uint32_t tlbhi) {
	__asm__ __volatile__ (
		"mts rtlbx, %0;\n\t"
		"mts rtlblo, %1;\n\t"
		"mts rtlbhi, %2;\n\t"
		:
		: "r"(tlbx),"r"(tlblo), "r" (tlbhi)
		: "memory"
	);
}

void get_utlb_record(int tlbx, uint32_t *tlblo, uint32_t *tlbhi) {
	uint32_t tmp1, tmp2;
	__asm__ __volatile__ (
		"mts rtlbx, %2;\n\t"
		"mfs %0, rtlblo;\n\t"
		"mfs %1, rtlbhi;\n\t"
		: "=r"(tmp1),"=r" (tmp2)
		: "r"(tlbx)
		: "memory"
	);
	*tlblo = tmp1;
	*tlbhi = tmp2;
}

static inline uint32_t reg_size_convert(size_t reg_size) {
	switch (reg_size) {
	case 0x400: /* 1kb */
		return RTLBHI_SIZE_1K;
	case 0x1000: /* 4k field */
		return RTLBHI_SIZE_4K;
	case 0x4000: /* 16k field */
		return RTLBHI_SIZE_16K;
	case 0x10000: /* 64k field */
		return RTLBHI_SIZE_64K;
	case 0x40000: /* 256 kb*/
		return RTLBHI_SIZE_256K;
	case 0x100000: /* 1M field */
		return RTLBHI_SIZE_1M;
	case 0x400000: /* 4M field*/
		return RTLBHI_SIZE_4M;
	case 0x1000000: /* 16M field */
		return RTLBHI_SIZE_16M;
	default: /* wrong size*/
		return -1;
	}
}

static inline int mmu_map_region(mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr,
		size_t reg_size, uint32_t flags) {
	uint32_t tlblo, tlbhi; /* mmu registers */
	uint32_t size_field;
	/* setup tlbhi register fields */
	size_field = reg_size_convert(reg_size);
	if (-1 == size_field) {
		return -1;
	}

	RTLBHI_SET(tlbhi, virt_addr, size_field);
	RTLBLO_SET(tlblo, phy_addr,
			((flags & PAGE_CACHEABLE) ? 1 : 0) ,
			((flags & PAGE_EXECUTEABLE) ? 1 : 0),
			((flags & PAGE_WRITEABLE) ? 1 : 0));

	set_utlb_record((cur_utlb_idx++) % UTLB_QUANTITY_RECORDS, tlblo, tlbhi);

	return cur_utlb_idx;
}

void mmu_set_val(void *addr, unsigned long val) {
	*((unsigned long *) addr) = val;
}

void mmu_set_context(mmu_ctx_t ctx) {
	cur_ctx = ctx;
	__asm__ __volatile__ (
		"mts rpid, %0"
		:
		: "r" (ctx)
		: "memory"
	);
}

void mmu_flush_tlb(void) {
}
