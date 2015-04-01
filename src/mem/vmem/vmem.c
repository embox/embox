/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */
#include <sys/mman.h>

#include <embox/unit.h>

#include <hal/mmu.h>
#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>
#include <mem/mapping/marea.h>
#include <mem/mmap.h>

#include <kernel/panic.h>

#include <kernel/task/resource/mmap.h>
#include <kernel/task/kernel_task.h>

static int mmu_enabled;

/* Section pointers. */
extern char _text_vma, _rodata_vma, _data_vma, _bss_vma;
extern char _text_len, _rodata_len, _data_len, _bss_len_with_reserve;

extern void mmap_add_marea(struct emmap *mmap, struct marea *marea);

void vmem_get_idx_from_vaddr(mmu_vaddr_t virt_addr, size_t *pgd_idx, size_t *pmd_idx, size_t *pte_idx) {
	*pgd_idx = ((uint32_t) virt_addr & MMU_PGD_MASK) >> MMU_PGD_SHIFT;
	*pmd_idx = ((uint32_t) virt_addr & MMU_PMD_MASK) >> MMU_PMD_SHIFT;
	*pte_idx = ((uint32_t) virt_addr & MMU_PTE_MASK) >> MMU_PTE_SHIFT;
}

static int vmem_kernel_map_marea(void *start, uint32_t len, uint32_t flags) {
	struct marea *ma;

	ma = marea_create((mmu_paddr_t) start, (mmu_paddr_t) start + len, flags, false);
	if (!ma) {
		return -1;
	}

	mmap_add_marea(task_resource_mmap(task_kernel_task()), ma);
	return 0;
}

void vmem_on(void) {
	mmu_on();
	mmu_enabled = 1;
}

void vmem_off(void) {
	mmu_off();
	mmu_enabled = 0;
}

int vmem_mmu_enabled(void) {
	return mmu_enabled;
}

int vmem_map_kernel(void) {
	int err = 0;

	/* Map sections. */
	err |= vmem_kernel_map_marea(&_text_vma, (size_t) &_text_len,
			PROT_WRITE | PROT_READ | PROT_EXEC);
	err |= vmem_kernel_map_marea(&_data_vma, (size_t) &_data_len,
			PROT_WRITE | PROT_READ);
	err |= vmem_kernel_map_marea(&_rodata_vma, (size_t) &_rodata_len,
			PROT_READ);
	err |= vmem_kernel_map_marea(&_bss_vma, (size_t) &_bss_len_with_reserve,
			PROT_WRITE | PROT_READ);

#if 0
	// XXX below
	// for sparc
	err |= vmem_map_on_itself(ctx, (void *) 0x80000000, (size_t) 0x100000,
			VMEM_PAGE_WRITABLE);

	// for sparc greth
	err |= vmem_map_on_itself(ctx, (void *) 0x90000000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);

	// for microblaze
	err |= vmem_map_on_itself(ctx, (void *) 0x84000000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);

	// mapping x86 video buffer
	err |= vmem_map_on_itself(ctx, (void *) 0xB8000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);
#endif

	return err;
}

void vmem_handle_page_fault(mmu_vaddr_t virt_addr) {
	panic("MMU page fault: virt_addr - 0x%x\n", (unsigned int) virt_addr);
}

