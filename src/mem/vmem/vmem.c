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
#include <stdio.h>
#include <hal/mm/mmu_core.h>
#include <hal/env/traps_core.h>
#include <hal/test/traps_core.h>
#include <mem/page.h>
#include "../kernel/task/common.h"
#include "../kernel/thread/types.h"
#include <mem/vmem.h>
#include <kernel/task/task_table.h>


extern char _mem_begin;
extern char _mem_length;

/* Section pointers. */
extern char _text_vma, _rodata_vma, _bss_vma, _data_vma, _stack_vma, _heap_vma;
extern char _text_len, _rodata_len, _bss_len, _data_len, _stack_len, _heap_len;

#define MEM_BEGIN       (&_mem_begin)
#define MEM_LENGTH      (&_mem_length)
#define MEM_END         (MEM_BEGIN + MEM_LENGTH)

#define USER_MEM_START	((vaddr_t) &_heap_vma + (vaddr_t) &_heap_len)
#define USER_MEM_SIZE   (3*1024*1024)

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
	/* one-on-one mapping for context 0 */
	mmu_map_region(ctx, (paddr_t)&_text_vma, (vaddr_t)&_text_vma, (size_t)&_text_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_rodata_vma, (vaddr_t)&_rodata_vma, (size_t)&_rodata_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_bss_vma, (vaddr_t)&_bss_vma, (size_t)&_bss_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_data_vma, (vaddr_t)&_data_vma, (size_t)&_data_len, MMU_PAGE_WRITEABLE);
	/* reserve section. */
	mmu_map_region(ctx, (paddr_t)&_data_vma + (paddr_t)&_data_len, (vaddr_t)&_data_vma + (vaddr_t)&_data_len, (size_t)&_stack_vma - (size_t)&_data_vma - (size_t)&_data_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_stack_vma, (vaddr_t)&_stack_vma, (size_t)&_stack_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(ctx, (paddr_t)&_heap_vma, (vaddr_t)&_heap_vma, (size_t)&_heap_len, MMU_PAGE_WRITEABLE);
}

static inline void vmem_create_space_after_kernel(mmu_ctx_t ctx) {
	int page_count;
	void *addr;

	page_count = USER_MEM_SIZE / MMU_PAGE_SIZE;
	addr = page_alloc(page_count);
	assert(addr != NULL);

	// Map INITIAL_SPACE after end of heap
	mmu_map_region(ctx, (paddr_t) addr, USER_MEM_START,
			USER_MEM_SIZE, MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
}

void vmem_create_virtual_space(mmu_ctx_t ctx) {
	vmem_map_kernel(ctx);
	vmem_create_space_after_kernel(ctx);
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

static int vmem_init(void) {
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
