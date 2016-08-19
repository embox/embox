/**
 * @file
 *
 * @date Aug 21, 2014
 * @author: Anton Bondarev
 */
#include <sys/mman.h>

#include <mem/vmem/vmem_alloc.h>
#include <mem/vmem.h>
#include <mem/mmap.h>
#include <mem/phymem.h>
#include <mem/mapping/marea.h>

#include <kernel/task/resource/mmap.h>

static const uint32_t mem_end = 0xFFFFF000;

uint32_t mmap_userspace_create(struct emmap *emmap, size_t stack_size) {
	struct marea *marea;
	void *phy_addr;

	marea = marea_create(mem_end - stack_size, mem_end, PROT_READ | PROT_WRITE | PROT_EXEC, false);
	mmap_add_marea(emmap, marea);

	phy_addr = phymem_alloc(stack_size / MMU_PAGE_SIZE);

	vmem_map_region(emmap->ctx, (mmu_paddr_t)phy_addr, mem_end - stack_size, stack_size, VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE);

	return (uint32_t)phy_addr;
}

void *mmap_userspace_add(void *addr, size_t len, int prot) {
	struct marea *marea;
	void *phy_addr;
	struct emmap *emmap;
	size_t pages;

	pages = (len + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
	phy_addr = phymem_alloc(pages);

	if (addr == NULL) {
		return phy_addr;
	}

	emmap = task_self_resource_mmap();

	marea = marea_create((uint32_t)addr, (uint32_t)addr + len, PROT_READ | PROT_WRITE | PROT_EXEC, true);
	mmap_add_marea(emmap, marea);

	vmem_map_region(emmap->ctx, (mmu_paddr_t)phy_addr, (uint32_t)addr, len, VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE);
	return addr;
}
