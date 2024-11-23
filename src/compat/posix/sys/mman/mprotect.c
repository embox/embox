/**
 * @file mprotect.c
 * @brief Set protection of memory mapping
 * @author Archit Checker <archit.checker5@gmail.com>
 * @version
 * @date 24.03.2020
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <hal/mmu.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>
#include <mem/vmem.h>

int mprotect(void *addr, size_t len, int prot) {
	int err;
	struct emmap *emmap = task_self_resource_mmap();

	if (((uintptr_t)addr % VMEM_PAGE_SIZE) != 0) {
		err = EINVAL;
		return SET_ERRNO(err);
	}

	prot &= PROT_READ | PROT_WRITE | PROT_EXEC;

	if ((err = vmem_set_flags(emmap->ctx, (mmu_vaddr_t)addr, len, prot))) {
		return err;
	}

	return 0;
}
