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
	int err;
	void *phy, *virt;
	struct emmap *emmap = task_self_resource_mmap();

	assert(emmap);

	log_debug("addr=%p, len=%d, prot=%d, flags=%d, fd=%d, off=%x", addr, len, prot, flags, fd, off);

	if (len == 0) {
		SET_ERRNO(EINVAL);
		return MAP_FAILED;
	}

	virt = (void *) binalign_bound((uint32_t) addr, VMEM_PAGE_SIZE);
	len = binalign_bound(len, VMEM_PAGE_SIZE);

	if (flags & MAP_ANONYMOUS) {
		/* Allocate physical memory and map it to `addr' */
		phy = phymem_alloc(len / VMEM_PAGE_SIZE);
		if (phy == NULL) {
			return MAP_FAILED;
		}

		if (virt == NULL) {
			/* OS should get unused address */
			virt = (void *) mmap_alloc(emmap, len);
		}

		if (vmem_translate(emmap->ctx, (mmu_vaddr_t) virt) != 0) {
			/* Virtual address is already taken, so return
			 * physical address instead */
			addr = phy;
		} else {
			vmem_map_region(emmap->ctx,
					(mmu_paddr_t) phy,
					(mmu_vaddr_t) virt,
					len,
					prot_to_vmem_flags(prot));
		}
	} else {
		assert(fd > 0);
		/* Call device-specific handler.
		 * Working with plain files is not supported for now */
		virt = phy = idesc_mmap(addr, len, prot, flags, fd, off);
	}

	if (phy == NULL) {
		return MAP_FAILED;
	}

	if ((err = vmem_set_flags(emmap->ctx,
					(mmu_vaddr_t) virt,
					len,
					prot_to_vmem_flags(prot)))) {
		log_error("Failed to set memory attributes for mmap()");
		return MAP_FAILED;
	}

	if (flags & MAP_ANONYMOUS) {
		prot |= MAP_ANONYMOUS;
	}

	if (mmap_place(emmap, (uintptr_t) virt, len, prot)) {
		return MAP_FAILED;
	}

	return virt;
}

int munmap(void *addr, size_t size) {
	struct emmap *emmap = task_self_resource_mmap();
	size_t len = binalign_bound(size, VMEM_PAGE_SIZE);

	if (mmap_prot(emmap, (uintptr_t) addr) & MAP_ANONYMOUS) {
		mmu_paddr_t phy_addr = vmem_translate(emmap->ctx, (mmu_vaddr_t) addr);
		vmem_unmap_region(emmap->ctx, (mmu_vaddr_t) addr, len);
		phymem_free((void *) phy_addr, len / VMEM_PAGE_SIZE);
	} else {
		/* TODO implement device-specific idesc_unmap? */
	}

	mmap_release(emmap, (uintptr_t) addr);

	return 0;
}
