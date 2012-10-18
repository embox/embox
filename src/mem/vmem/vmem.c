/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <assert.h>
#include <errno.h>
#include <types.h>
#include <prom/prom_printf.h>
#include <hal/mmu.h>
#include <mem/vmem.h>
#include <mem/vmem/virtalloc.h>

#include "../kernel/task/common.h"
#include "../kernel/thread/types.h"

/* Section pointers. */
extern char _text_vma, _rodata_vma, _bss_vma, _data_vma, _reserve_vma, _stack_vma, _heap_vma;
extern char _text_len, _rodata_len, _bss_len, _data_len, _reserve_len, _stack_len, _heap_len;

EMBOX_UNIT(vmem_init, vmem_fini);

static int initialized = 0;
static inline int vmem_map_kernel(mmu_ctx_t ctx);
static inline int vmem_create_context(struct task *task);

int vmem_on(void) {
	int err = 0;
	struct task *task = task_self();

	initialized = 1;
	/* Create context for init. */
	if ((err = vmem_create_context(task))) {
		return err;
	}

	// TODO: Change it.
	switch_mm((mmu_ctx_t) 0, task->vmem_data->ctx);
	mmu_on();

	return ENOERR;
}

void vmem_off(void) {
	mmu_off();
}

static inline int vmem_map_on_itself(mmu_ctx_t ctx, void *addr, size_t size, vmem_page_flags_t flags) {
	return vmem_map_region(ctx, (mmu_paddr_t) addr, (mmu_vaddr_t) addr, (size_t) size, flags);
}

static inline int vmem_map_kernel(mmu_ctx_t ctx) {
	int err = 0;

	/* Map sections. */
	err |= vmem_map_on_itself(ctx, &_text_vma, (size_t) &_text_len, VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_rodata_vma, (size_t) &_rodata_len, VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_bss_vma, (size_t) &_bss_len, VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_data_vma, (size_t) &_data_len, VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_reserve_vma, (size_t) &_reserve_len, VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_stack_vma, (size_t) &_stack_len, VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_heap_vma, (size_t) &_heap_len, VMEM_PAGE_WRITABLE);

	/* Map special info. */
	err |= vmem_map_on_itself(ctx, VIRTUAL_TABLES_START, VIRTUAL_TABLES_LEN, VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, VIRTUAL_PAGES_INFO_START, VIRTUAL_PAGES_INFO_LEN, VMEM_PAGE_WRITABLE);

	// for sparc
	err |= vmem_map_on_itself(ctx, (void *) 0x80000000, (size_t) 0x1000, VMEM_PAGE_WRITABLE);

	return err;
}

static inline int vmem_create_context(struct task *task) {
	task->vmem_data->ctx = mmu_create_context((mmu_pgd_t *) virt_alloc_table());
	return vmem_map_kernel(task->vmem_data->ctx);
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

static inline void vmem_print_info(void) {
	prom_printf("\n");
	prom_printf("MMU_PTE_MASK = 0x%08x\n", (unsigned int) MMU_PTE_MASK);
	prom_printf("MMU_PMD_MASK = 0x%08x\n", (unsigned int) MMU_PMD_MASK);
	prom_printf("MMU_PGD_MASK = 0x%08x\n", (unsigned int) MMU_PGD_MASK);

	prom_printf("\n");
	prom_printf("   text: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_text_vma, (unsigned int) &_text_len);
	prom_printf(" rodata: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_rodata_vma, (unsigned int) &_rodata_len);
	prom_printf("    bss: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_bss_vma, (unsigned int) &_bss_len);
	prom_printf("   data: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_data_vma, (unsigned int) &_data_len);
	prom_printf("reserve: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_reserve_vma, (unsigned int) &_reserve_len);
	prom_printf("  stack: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_stack_vma, (unsigned int) &_stack_len);
	prom_printf("   heap: start = 0x%08x, size = 0x%08x\n", (unsigned int) &_heap_vma, (unsigned int) &_heap_len);
}


static int vmem_init(void) {
	//vmem_print_info();
	return vmem_on();
}

static int vmem_fini(void) {
	vmem_off();

	return 0;
}

static void task_vmem_init(struct task *task, void *_vmem_data) {
	task->vmem_data = (struct task_vmem_data *) _vmem_data;;

	/*
	 * XXX:
	 * Init task initializes in vmem_on(), because this module can be not
	 * loaded yet.
	 */
	if (initialized) {
		vmem_create_context(task);
	}
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
