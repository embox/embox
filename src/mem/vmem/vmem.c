/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 29.07.11
 * @author Anton Kozlov
 */

#include <embox/unit.h>

#include <assert.h>
#include <types.h>
#include <prom/prom_printf.h>
#include <hal/mmu.h>

#if 0
#include "../kernel/task/common.h"
#include "../kernel/thread/types.h"
#endif

EMBOX_UNIT(vmem_init, vmem_fini);

static int vmem_init(void) {
	prom_printf("MMU_PTE_MASK = 0x%08x", MMU_PTE_MASK);
	prom_printf("MMU_PMD_MASK = 0x%08x", MMU_PMD_MASK);
	prom_printf("MMU_PGD_MASK = 0x%08x", MMU_PGD_MASK);

	return 0;
}

static int vmem_fini(void) {


	return 0;
}


#if 0
/* Section pointers. */
extern char _text_vma, _rodata_vma, _bss_vma, _data_vma, _stack_vma, _heap_vma;
extern char _text_len, _rodata_len, _bss_len, _data_len, _stack_len, _heap_len;

// TODO: do it normal
#define _reserve_vma ((paddr_t) &_data_vma + (paddr_t) &_data_len)
#define _reserve_len ((size_t) &_stack_vma - (size_t) &_data_vma - (size_t) &_data_len)

#define USER_MEM_START	((vaddr_t) &_heap_vma + (vaddr_t) &_heap_len)
#define USER_MEM_SIZE   (2*1024*1024)


EMBOX_UNIT(vmem_init, vmem_fini);

static inline void vmem_map_kernel(mmu_ctx_t ctx);
static inline void vmem_create_space_after_kernel(mmu_ctx_t ctx);

void vmem_on(void) {
	switch_mm((mmu_ctx_t) 0, task_self()->vmem_data->ctx);
	mmu_on();
}

void vmem_off(void) {
	mmu_off();
}

static inline void vmem_map_kernel(mmu_ctx_t ctx) {
	mmu_map_region(ctx, (paddr_t)&_text_vma, (vaddr_t)&_text_vma, (size_t)&_text_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_rodata_vma, (vaddr_t)&_rodata_vma, (size_t)&_rodata_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_bss_vma, (vaddr_t)&_bss_vma, (size_t)&_bss_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_data_vma, (vaddr_t)&_data_vma, (size_t)&_data_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t) _reserve_vma, (vaddr_t) _reserve_vma, (size_t) _reserve_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_stack_vma, (vaddr_t)&_stack_vma, (size_t)&_stack_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_heap_vma, (vaddr_t)&_heap_vma, (size_t)&_heap_len, MMU_PAGE_WRITEABLE);

	// for sparc
	mmu_map_region(ctx, (paddr_t) 0x80000000, (vaddr_t) 0x80000000, (size_t) 0x4000, MMU_PAGE_WRITEABLE);
}

static inline void vmem_create_space_after_kernel(mmu_ctx_t ctx) {
	void *addr = alloc_virt_memory((size_t) USER_MEM_SIZE);
	assert(addr);

	// Map space after kernel
	mmu_map_region(ctx, (paddr_t) addr, USER_MEM_START,
			USER_MEM_SIZE, MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
}

void vmem_create_virtual_space(mmu_ctx_t ctx) {
	vmem_map_kernel(ctx);
	//vmem_create_space_after_kernel(ctx);
}

/*

static int pagefault_handler(uint32_t nr, void *data) {
	int err_addr = mmu_get_fault_address() & ~(PAGE_SIZE() - 1);
	mmu_off();

	mmu_map_region((mmu_ctx_t) 0, (paddr_t) page_alloc() ,
			(vaddr_t) err_addr & ~(PAGE_SIZE() - 1), PAGE_SIZE(), MMU_PAGE_CACHEABLE |
			MMU_PAGE_WRITEABLE);

	mmu_on();
	return 1; // execute exception-cause instruction once more
}

*/

static inline void vmem_print_kernel_mapping(void) {
	printf("\n");
	printf("   text: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_text_vma, (unsigned int) &_text_len);
	printf(" rodata: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_rodata_vma, (unsigned int) &_rodata_len);
	printf("    bss: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_bss_vma, (unsigned int) &_bss_len);
	printf("   data: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_data_vma, (unsigned int) &_data_len);
	printf("reserve: start = 0x%08x, size = 0x%08x\n", (unsigned int) _reserve_vma, (unsigned int) _reserve_len);
	printf("  stack: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_stack_vma, (unsigned int) &_stack_len);
	printf("   heap: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_heap_vma, (unsigned int) &_heap_len);
}


static int vmem_init(void) {
	vmem_print_kernel_mapping();

	vmem_on();

	return 0;
}

static int vmem_fini(void) {
	vmem_off();

	return 0;
}

static void task_vmem_init(struct task *task, void *_vmem_data) {
	struct task_vmem_data *vmem_data = (struct task_vmem_data *) _vmem_data;
	task->vmem_data = vmem_data;

	vmem_data->ctx = mmu_create_context();
	vmem_create_virtual_space(vmem_data->ctx);
}

static void task_vmem_inherit(struct task *task, struct task *parent_task) {

}

static void task_vmem_deinit(struct task *task) {

}

static const struct task_resource_desc vmem_resource = {
	.init = task_vmem_init,
	.inherit = task_vmem_inherit,
	.deinit = task_vmem_deinit,
	.resource_size = sizeof(struct task_vmem_data),
};

static int task_switch_handler(struct thread *prev, struct thread *next) {
	switch_mm(prev->task->vmem_data->ctx, next->task->vmem_data->ctx);

	return 0;
}

TASK_RESOURCE_DESC(&vmem_resource);
TASK_RESOURCE_NOTIFY(task_switch_handler);

#endif
