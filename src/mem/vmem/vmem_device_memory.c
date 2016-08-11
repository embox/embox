/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */


#include <stddef.h>
#include <sys/mman.h>

#include <hal/mmu.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mapping/marea.h>
#include <mem/mmap.h>
#include <mem/phymem.h>

static struct emmap *self_mmap(void) {
	if (0 == mmap_kernel_inited()) {
		return mmap_early_emmap();
	} else {
		return task_self_resource_mmap();
	}
}

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
	struct emmap *emmap = self_mmap();
	struct marea *marea;

	marea = marea_create(physical & ~MMU_PAGE_MASK,
			(len + physical + MMU_PAGE_MASK) & ~MMU_PAGE_MASK,
			prot, false);

	if (NULL == marea) {
		return NULL;
	}
	mmap_add_marea(emmap, marea);

	if (mmap_kernel_inited()) {
		mmap_do_marea_map(emmap, marea);
	}

	return addr;
}

int munmap(void *addr, size_t size) {
	struct emmap *emmap = self_mmap();
	struct marea *marea;

	marea = mmap_find_marea(emmap, (mmu_vaddr_t) addr);
	if (NULL == marea) {
		return SET_ERRNO(ENOENT);
	}

	mmap_del_marea(marea);

	if (mmap_kernel_inited()) {
		mmap_do_marea_unmap(emmap, marea);
	}

	if (marea->is_allocated) {
		int pages = (size + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
		phymem_free((void *)(marea->start), pages);
	}

	marea_destroy(marea);
	return 0;
}
