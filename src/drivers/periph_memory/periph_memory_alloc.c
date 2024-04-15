/**
 * @file
 *
 * @date Mar 12, 2020
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <mem/vmem.h>
#include <mem/sysmalloc.h>
#include <module/embox/arch/mmu.h>

#include <drivers/common/periph_memory_alloc.h>

#ifndef NOMMU

#include <drivers/common/memory.h>
#include <hal/mmu.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>

#include <mem/mmap.h>

void *periph_memory_alloc(size_t len) {
	void *mem = sysmemalign(MMU_PAGE_SIZE, len);
	if (!mem) {
		return NULL;
	}

	vmem_set_flags(vmem_current_context(),
			(mmu_vaddr_t) mem,
			len,
			PROT_WRITE | PROT_READ | PROT_NOCACHE);

	mmu_flush_tlb();

	return mem;
}
#else
void *periph_memory_alloc(size_t len) {
	return sysmemalign(sizeof(void *), len);
}
#endif /* NOMMU */

void periph_memory_free(void *ptr) {
	sysfree(ptr);
}
