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
#include <kernel/printk.h>

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t*) (((uint32_t)*pgd) & ~MMU_PAGE_MASK);
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t *pmd) {
	return pmd;
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return ((uint32_t) *pte) & ~MMU_PAGE_MASK;
}

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
	*pgd = (mmu_pgd_t) ((((uint32_t)pmd) & ~MMU_PAGE_MASK)
		| ARM_MMU_TYPE_PAGE);

}

void mmu_pmd_set(mmu_pgd_t *pmd, mmu_pmd_t *pte) {
}

void mmu_pte_set(mmu_pte_t *pte, mmu_paddr_t addr) {
	*pte = (mmu_pte_t) ((((uint32_t)addr) & ~MMU_PAGE_MASK)
		| L1D_TYPE_SD | ARM_MMU_PAGE_READ_ACC);
}

void mmu_pgd_unset(mmu_pgd_t *pgd) {
	*pgd = 0x0;
}

void mmu_pmd_unset(mmu_pgd_t *pmd) {
	*pmd = 0x0;
}

void mmu_pte_unset(mmu_pgd_t *pte) {
	*pte = 0x0;
}

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return (((uint32_t)*pgd) & ARM_MMU_TYPE_PAGE) == ARM_MMU_TYPE_PAGE;
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return (((uint32_t)*pmd) & ARM_MMU_SMALL_PAGE) == ARM_MMU_SMALL_PAGE;
}

int mmu_pte_present(mmu_pte_t *pte) {
	return (((uint32_t)*pte) & ARM_MMU_SMALL_PAGE) == ARM_MMU_SMALL_PAGE;
}

void mmu_pte_set_writable(mmu_pte_t *pte, int value) {
	if (value & VMEM_PAGE_WRITABLE) {
		*pte |= ARM_MMU_PAGE_WRITE_ACC;
	}
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int value) {
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

}

void mmu_pte_set_usermode(mmu_pte_t *pte, int value) {
}

void mmu_pte_set_executable(mmu_pte_t *pte, int value) {
#if 0
	if (!(value & VMEM_PAGE_EXECUTABLE)) {
		*pte |= L1D_XN;
	} else {
		*pte &= ~L1D_XN;
	}
#endif
}

#define L1_ADDR_MASK 0xFFFFFC00
#define L2_ADDR_MASK 0xFFF

static uint32_t *get_l1_desc(uint32_t virt_addr)
{
	uint32_t *_translation_table = (void*)vmem_current_context();
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

    printk("vmem info: base %p; vaddr %p", (void*) vmem_current_context(), (void *) vaddr);

    if (*l1_desc == 0x0) {
        printk(" unmapped l1_desc = 0x%" PRIx32 "\n", *l1_desc);
        return 0x0;
    }

    l2_desc = get_l2_desc(vaddr, *l1_desc);
    printk("->%p desc(%p);\n", (void *) ((*l2_desc) & ~L2_ADDR_MASK), (void *) (*l2_desc));
    printk("l1 desc %p\n", (void*) *l1_desc);
    return ((*l2_desc) & ~L2_ADDR_MASK);
}
