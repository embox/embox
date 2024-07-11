/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <hal/mmu.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>
#include <mem/vmem.h>
#include <util/binalign.h>
#include <util/log.h>

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
void *mmap_device_memory(void *addr, size_t len, int prot, int flags,
    uintptr_t physical) {
	/* TODO handle addr=NULL, region should be mapped to any addr
	 * TODO handle flags anyhow */
	uintptr_t start = (physical & ~(uintptr_t)MMU_PAGE_MASK);
	uintptr_t end = binalign_bound(physical + len, MMU_PAGE_SIZE);

	len = end - start;

	if (mmap_place(task_self_resource_mmap(), start, len, prot)) {
		/* it is not a error because this region can be mapped earlier
		 *
		 */
		/* TODO mmap_device_memory must return shared address it available
		 log_error("mmap_place() addr(%p), len(%zu) failed",
				(uintptr_t)physical, len);
		*/
		return NULL;
	}

	vmem_map_region(vmem_current_context(), start, start, len, prot);
	log_debug("phy (0x%" PRIxPTR ") mapped to %p", start, addr);

	return addr;
}
