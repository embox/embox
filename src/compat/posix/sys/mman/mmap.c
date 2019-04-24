/**
 * @file mmap.c
 * @brief Various memory mapping
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.02.2018
 */

#include <errno.h>
#include <stddef.h>
#include <sys/mman.h>

#include <hal/mmu.h>
#include <mem/phymem.h>
#include <mem/vmem.h>
#include <mem/mmap.h>
#include <mem/mapping/marea.h>
#include <kernel/task/resource/mmap.h>
#include <module/embox/fs/syslib/idesc_mmap_api.h>
#include <util/binalign.h>
#include <util/log.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	struct marea *marea;
	int err;
	void *ptr;
	struct emmap *emmap = task_self_resource_mmap();

	assert(emmap);

	log_debug("addr=%p, len=%d, prot=%d, flags=%d, fd=%d, off=%x", addr, len, prot, flags, fd, off);

	if (len == 0) {
		SET_ERRNO(EINVAL);
		return MAP_FAILED;
	}

	addr = (void *) binalign_bound((uint32_t) addr, VMEM_PAGE_SIZE);
	len = binalign_bound(len, VMEM_PAGE_SIZE);

	if (flags & MAP_ANONYMOUS) {
		/* Allocate physical memory and map it to `addr' */
		ptr = phymem_alloc(len / VMEM_PAGE_SIZE);

		if (ptr == NULL) {
			return MAP_FAILED;
		}

		if (vmem_translate(emmap->ctx, (mmu_vaddr_t) addr) != 0) {
			/* Virtual address is already taken, so return
			 * physical address instead */
			addr = ptr;
		} else {
			vmem_map_region(emmap->ctx,
					(mmu_paddr_t) ptr,
					(mmu_vaddr_t) addr,
					len,
					marea_to_vmem_flags(prot));
		}
	} else {
		assert(fd > 0);
		/* Call device-specific handler.
		 * Working with plain files is not supported for now */
		addr = ptr = idesc_mmap(addr, len, prot, flags, fd, off);
	}

	if (ptr == NULL) {
		return MAP_FAILED;
	}

	if ((err = vmem_set_flags(emmap->ctx,
					(mmu_vaddr_t) addr,
					len,
					marea_to_vmem_flags(prot)))) {
		log_error("Failed to set memory attributes for mmap()");
		return MAP_FAILED;
	}

	if (NULL == (marea = marea_create(
			(uintptr_t) ptr, (uintptr_t) ptr + len, prot, 0))) {
		return MAP_FAILED;
	}

	if (flags & MAP_ANONYMOUS) {
		marea->flags |= MAP_ANONYMOUS;
	}

	mmap_add_marea(emmap, marea);

	return ptr;
}

int munmap(void *addr, size_t size) {
	struct emmap *emmap = task_self_resource_mmap();
	struct marea *marea = NULL, *m;
	size_t len = binalign_bound(size, VMEM_PAGE_SIZE);

	do {
		marea = mmap_find_marea_next(emmap, (mmu_vaddr_t) addr, marea);
		if (NULL == marea) {
			return SET_ERRNO(ENOENT);
		}
	} while (len != marea->end - marea->start);

	if (marea->flags & MAP_ANONYMOUS) {
		mmu_paddr_t phy_addr = vmem_translate(emmap->ctx, (mmu_vaddr_t) addr);

		phymem_free((void *) phy_addr, len / VMEM_PAGE_SIZE);
	} else {
		/* TODO implement device-specific idesc_unmap? */
	}

	mmap_del_marea(marea);

	if (mmap_kernel_inited()) {
		marea_destroy(marea);

		/* TODO calculate possible overlap more accurate */
		if ((m = mmap_find_marea(emmap, (mmu_vaddr_t) addr))) {
			/* Another marea for context overlaps this memory segment,
			 * so we should restore old memory attributes */
			mmap_do_marea_map_overwrite(emmap, m);
		} else {
			mmap_do_marea_unmap(emmap, marea);
		}
	}


	return 0;
}
