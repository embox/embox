/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-25
 */

#include <string.h>
#include <inttypes.h>

#include <asm/hal/mmu.h>
#include <hal/mmu.h>
#include <mem/vmem.h>

#include <util/log.h>

#define L1_ADDR_MASK    0xFFFFFC00
#define L2_ADDR_MASK    0xFFFFF000
#define TTBR0_ADDR_MASK 0xFFFFFF00

extern void dcache_flush(const void *p, size_t size);

uintptr_t *mmu_value(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
		return (uintptr_t *) (*entry & L1_ADDR_MASK);
	case 1:
		return (uintptr_t *) (*entry & L2_ADDR_MASK);
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	switch (lvl) {
	case 0:
		*entry = (value & L1_ADDR_MASK) | ARM_MMU_TYPE_PAGE;
		break;
	case 1:
		*entry = (value & L2_ADDR_MASK) | L1D_TYPE_SD | ARM_MMU_PAGE_READ_ACC;
		break;
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		break;
	}

	dcache_flush(entry, sizeof(*entry));
}

void mmu_unset(int lvl, uintptr_t *entry) {
	*entry = 0;
	dcache_flush(entry, sizeof(*entry));
}

int mmu_present(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
		return (((uint32_t)*entry) & ARM_MMU_TYPE_PAGE) == ARM_MMU_TYPE_PAGE;
	case 1:
		return (((uint32_t)*entry) & ARM_MMU_SMALL_PAGE) == ARM_MMU_SMALL_PAGE;
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

void mmu_pte_set_writable(uintptr_t *pte, int value) {
	if (value & VMEM_PAGE_WRITABLE) {
		*pte |= ARM_MMU_PAGE_WRITE_ACC;
	}
	dcache_flush(pte, sizeof(*pte));
}

void mmu_pte_set_cacheable(uintptr_t *pte, int value) {
	*pte &= ~(L1D_B | L1D_C);
#ifndef V5_FORMAT
	*pte &= ~L1D_TEX_MASK;
#endif
	if (value & VMEM_PAGE_CACHEABLE) {
#ifndef V5_FORMAT
		*pte |= L1D_TEX_USE;   /* Outer strongly-ordered memory */
#endif
		*pte |= L1D_C | L1D_B; /* Inner write-through cached memory */
	} else {
		*pte |= L1D_B;         /* Shareable device memory */
	}

	dcache_flush(pte, sizeof(*pte));
}

void mmu_pte_set_usermode(uintptr_t *pte, int value) {
}

void mmu_pte_set_executable(uintptr_t *pte, int value) {
#if 0
	if (!(value & VMEM_PAGE_EXECUTABLE)) {
		*pte |= L1D_XN;
	} else {
		*pte &= ~L1D_XN;
	}
#endif
}

/* vmem_info */
#include <kernel/printk.h>

extern uint32_t arm_get_ttbr0(void);

static uint32_t *get_l1_desc(uint32_t virt_addr)
{
	uint32_t *_translation_table = (void *) (arm_get_ttbr0() & TTBR0_ADDR_MASK);
	int l1_idx = virt_addr >> 20;
	return &_translation_table[l1_idx];
}

static uint32_t *get_l2_desc( uint32_t virt_addr, uint32_t l1_desc)
{
    uint32_t *l2_tbl = (uint32_t*)(l1_desc & L1_ADDR_MASK);
    int l2_idx = (virt_addr >> 12) & 0xFF;
    return &l2_tbl[l2_idx];
}

uint32_t vmem_info(uint32_t vaddr)
{
    uint32_t *l1_desc = get_l1_desc(vaddr);
    printk("l1 desc %p\n", (void*) *l1_desc);
    uint32_t *l2_desc;

    vaddr &= ~0xFFF;

    printk("vmem info: base %p; vaddr %p",
		    (void *) (arm_get_ttbr0() & TTBR0_ADDR_MASK),
		    (void *) vaddr);

    if (*l1_desc == 0x0) {
        printk(" unmapped l1_desc = 0x%" PRIx32 "\n", *l1_desc);
        return 0x0;
    }

    l2_desc = get_l2_desc(vaddr, *l1_desc);
    printk("->%p desc(%p);\n", (void *) ((*l2_desc) & L2_ADDR_MASK), (void *) (*l2_desc));
    printk("l1 desc %p\n", (void*) *l1_desc);
    return ((*l2_desc) & L2_ADDR_MASK);
}
