/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */

#include <util/binalign.h>
#include <util/log.h>

#include <stddef.h>
#include <sys/mman.h>

#include <hal/mmu.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>
#include <mem/vmem.h>

/**
 * @brief Map specific memory address to access device
 *
 * @param addr Virtual address to where you want to map the object
 * @param len Number of bytes to be mapped
 * @param prot Access flags for region (PROT_EXEC/PROT_NOCACHE/etc)
 * @param flags MAP_FIXED/etc
 * @param physical Physical address of memory that's to be mapped
 *
 * @return Pointer to allocated vmem or NULL if failed
 */
void *mmap_device_memory(void *addr,
		size_t len,
		int prot,
		int flags,
		uint64_t physical){
	/* TODO handle addr=NULL, region should be mapped to any addr
	 * TODO handle flags anyhow */
	uintptr_t start = (uintptr_t) (physical & ~MMU_PAGE_MASK);
	uintptr_t end = binalign_bound(physical + len, MMU_PAGE_SIZE);
	len = end - start;

	if (mmap_place(task_self_resource_mmap(), start, len, prot)) {
		return NULL;
	}

	vmem_map_region(vmem_current_context(),
				start,
				start,
				len,
				prot);

	return addr;
}
