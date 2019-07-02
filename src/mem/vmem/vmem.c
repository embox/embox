/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 * @author Anton Bondarev
 */

#include <framework/mod/options.h>
#include <util/log.h>

#include <stdint.h>
#include <sys/mman.h>
#include <inttypes.h>

#include <embox/unit.h>
#include <hal/mmu.h>
#include <kernel/panic.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/task/kernel_task.h>
#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>
#include <mem/mmap.h>

#include <util/binalign.h>
#include <util/math.h>

/* Section pointers. */
extern char _text_vma, _rodata_vma, _data_vma, _bss_vma;
extern char _text_len, _rodata_len, _data_len, _bss_len_with_reserve;

int vmem_map_kernel(void) {
	int err = 0;
	uintptr_t kernel_map_start;
	uintptr_t kernel_map_end;
	size_t kernel_map_size;
	/* Map sections. */

	kernel_map_start = (uintptr_t) min(
			min(&_text_vma, &_data_vma),
			min(&_rodata_vma, &_bss_vma)
		) & ~MMU_PAGE_MASK;

	kernel_map_end = (uintptr_t) max(
			max(&_text_vma + (size_t) &_text_len,
				&_data_vma + (size_t) &_data_len),
			max(&_rodata_vma + (size_t) &_rodata_len,
				&_bss_vma + (size_t) &_bss_len_with_reserve));

	kernel_map_size = binalign_bound(kernel_map_end - kernel_map_start,
			MMU_PAGE_SIZE);
	log_debug("kernel mapping at 0x%" PRIxPTR " size 0x%" PRIxPTR " flags 0x%" PRIx32 "",
			kernel_map_start,
			kernel_map_size,
			PROT_WRITE | PROT_READ | PROT_EXEC);

	err = mmap_place(task_resource_mmap(task_kernel_task()),
				kernel_map_start,
				kernel_map_size,
				PROT_WRITE | PROT_READ | PROT_EXEC);
	return err;
}

EMBOX_UNIT_INIT(vmem_init);
static int vmem_init(void) {
	int ret;
	struct marea *marea;
	struct emmap *emmap;
	struct task *task;

	ret = vmem_map_kernel();
	assert(ret == 0);

	log_debug("kernel has been successfully mapped");
	task_foreach(task) {
		emmap = task_resource_mmap(task);
		dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
			log_debug("map region (base 0x%" PRIxPTR " size %zu flags 0x%" PRIx32 ")",
					marea->start, marea->size, marea->flags);
			if (vmem_map_region(emmap->ctx,
					marea->start,
					marea->start,
					marea->size,
					marea->flags)) {
				panic("Failed to initialize kernel memory mapping");
			}
		}
	}

	emmap = task_resource_mmap(task_kernel_task());
	mmu_set_context(emmap->ctx);

	mmu_on();

	return 0;
}
