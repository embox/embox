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

/**
 * @brief Try to munmap MAP_ANONYMOUS memory
 *
 * @param addr
 * @param size
 *
 * @return 1 if requested to unmapp not MAP_ANONYMOUS mapping
 * @return 0 if successfully unmapped
 * @return negative error if this is MAP_ANONYMOUS memory,
 * 	but error arise during unmapping
 */
static int munmap_anon(void *addr, size_t size) {
	struct page_allocator *anon_pages = __phymem_allocator;
	struct emmap *emmap = self_mmap();
	struct phy_page *phy_page;

	if (!page_belong(anon_pages, addr) || !page_belong(anon_pages, addr + size)) {
		return 1;
	}

	void *start = (void *)((uintptr_t)addr & -MMU_PAGE_SIZE);
	/* below is complex to handle addr not page-aligned, but size page-aligned
	 * actual number of pages is (size + 1)/MMU_PAGE_SIZE */
	int pages = (addr + size - start + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;

	phy_page = mmap_find_phy_page(emmap, start);
	if (NULL == phy_page) {
		return SET_ERRNO(ENOENT);
	}

	if (pages != phy_page->page_number) {
		return SET_ERRNO(EINVAL);
	}

	phymem_free(start, pages);

	mmap_del_phy_page(phy_page);
	phy_page_destroy(phy_page);

	return 0;
}

int munmap(void *addr, size_t size) {
	struct emmap *emmap = self_mmap();
	struct marea *marea;

	int anon_res = munmap_anon(addr, size);
	if (1 != anon_res) {
		return anon_res;
	}

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
