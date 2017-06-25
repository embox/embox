/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>
#include <hal/mmu.h>
#include <kernel/panic.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/task/kernel_task.h>
#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>
#include <mem/mapping/marea.h>
#include <mem/mmap.h>
#include <sys/mman.h>
#include <util/binalign.h>
#include <util/math.h>

static int mmu_enabled;

/* Section pointers. */
extern char _text_vma, _rodata_vma, _data_vma, _bss_vma;
extern char _text_len, _rodata_len, _data_len, _bss_len_with_reserve;

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

	uintptr_t kernel_map_start = (uintptr_t) min(
			min(&_text_vma, &_data_vma),
			min(&_rodata_vma, &_bss_vma)
		) & ~MMU_PAGE_MASK;

	uintptr_t kernel_map_end = (uintptr_t) max(
			max(	&_text_vma + (size_t) &_text_len,
				&_data_vma + (size_t) &_data_len),
			max(	&_rodata_vma + (size_t) &_rodata_len,
				&_bss_vma + (size_t) &_bss_len_with_reserve));

	err = vmem_kernel_map_marea(
		(void*) kernel_map_start,
		binalign_bound(kernel_map_end - kernel_map_start, MMU_PAGE_SIZE),
		PROT_WRITE | PROT_READ | PROT_EXEC);

	return err;
}

void vmem_handle_page_fault(mmu_vaddr_t virt_addr) {
	panic("MMU page fault: virt_addr - 0x%x\n", (unsigned int) virt_addr);
}

