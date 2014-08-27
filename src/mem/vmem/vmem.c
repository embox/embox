/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */
#include <sys/mman.h>

#include <embox/unit.h>

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
/* phymem allocator space */
extern char *phymem_alloc_start, *phymem_alloc_end;

extern void mmap_add_marea(struct emmap *mmap, struct marea *marea);

static int vmem_kernel_map_marea(void *start, uint32_t len, uint32_t flags) {
	struct marea *ma;

	ma = marea_create((mmu_paddr_t) start, (mmu_paddr_t) start + len, flags);
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
	size_t phymem_len;

	/* Map sections. */
	err |= vmem_kernel_map_marea(&_text_vma, (size_t) &_text_len,
			PROT_WRITE | PROT_READ | VMEM_PAGE_EXECUTABLE);
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
#endif
	// mapping x86 video buffer
//	err |= vmem_map_on_itself(ctx, (void *) 0xB8000, (size_t) 0x1000,
//			VMEM_PAGE_WRITABLE);

	// map phymem
	phymem_len = phymem_alloc_end - phymem_alloc_start;
	err |= vmem_kernel_map_marea(phymem_alloc_start, phymem_len, VMEM_PAGE_WRITABLE);

	return err;
}

void vmem_handle_page_fault(mmu_vaddr_t virt_addr) {
	panic("MMU page fault: virt_addr - 0x%x\n", virt_addr);
}
