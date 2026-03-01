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
#include <mem/vmem_device_memory.h>
#include <util/binalign.h>
#include <util/log.h>

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
