/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <errno.h>
#include <assert.h>
#include <string.h>

#include <hal/mmu.h>
#include <util/binalign.h>
#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>

struct mmu_entry {
	uintptr_t *table[MMU_LEVELS];
	int idx[MMU_LEVELS];
};

static void vmem_set_pte_flags(uintptr_t *pte, vmem_page_flags_t flags) {
	mmu_pte_set_writable(pte, flags & VMEM_PAGE_WRITABLE);
	mmu_pte_set_executable(pte, flags & VMEM_PAGE_EXECUTABLE);
	mmu_pte_set_cacheable(pte, flags & VMEM_PAGE_CACHEABLE);
	mmu_pte_set_usermode(pte, flags & VMEM_PAGE_USERMODE);
}

static struct mmu_entry *vmem_entry_get_idxs(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, struct mmu_entry *entry) {
	int i;

	for (i = 0; i < MMU_LEVELS; i++) {
		entry->idx[i] = (virt_addr & MMU_MASK(i)) >> MMU_SHIFT(i);
	}
	return entry;
}

static struct mmu_entry *vmem_entry_get_tables(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, struct mmu_entry *entry) {
	int i;

	entry->table[0] = mmu_get_root(ctx);

	for (i = 1; i < MMU_LEVELS; i++) {
		entry->table[i] = mmu_value(i - 1, entry->table[i - 1] + entry->idx[i - 1]);
		if (entry->table[i] == NULL) {
			break;
		}
	}

	return entry;
}

static struct mmu_entry *vmem_entry_from_vaddr(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, struct mmu_entry *entry) {
	vmem_entry_get_idxs(ctx, virt_addr, entry);
	vmem_entry_get_tables(ctx, virt_addr, entry);

	return entry;
}

static int vmem_entry_set_pte(struct mmu_entry *entry, mmu_paddr_t phy_addr, vmem_page_flags_t flags) {
	int i = 0;

	assert(entry);

	for (i = 0; i < MMU_LAST_LEVEL; i++) {
		if (!mmu_present(i, entry->table[i] + entry->idx[i])) {
			if (!(entry->table[i + 1] = vmem_alloc_table(i + 1))) {
				log_error("Failed to alloc table lvl%d\n", i);
			}

			mmu_set(i, entry->table[i] + entry->idx[i], (uintptr_t) entry->table[i + 1]);
		}
	}

	mmu_set(MMU_LAST_LEVEL, entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL], phy_addr);
	vmem_set_pte_flags(entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL], flags);

	return 0;
}

static int vmem_entry_try_free(struct mmu_entry *entry) {

	if (entry->table[MMU_LAST_LEVEL]) {
		mmu_unset(MMU_LAST_LEVEL, entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL]);
	}

	return 0;
}

int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr,
		size_t reg_size, vmem_page_flags_t flags) {
	struct mmu_entry entries;

	for ( ; reg_size; reg_size -= MMU_PAGE_SIZE) {
		vmem_entry_get_idxs(ctx, virt_addr, &entries);
		vmem_entry_get_tables(ctx, virt_addr, &entries);

		vmem_entry_set_pte(&entries, phy_addr, flags);
		phy_addr += MMU_PAGE_SIZE;
		virt_addr += MMU_PAGE_SIZE;
	}

	mmu_flush_tlb();

	return 0;

}

mmu_paddr_t vmem_translate(mmu_ctx_t ctx, mmu_vaddr_t virt_addr) {
	uintptr_t *pte;
	struct mmu_entry entries;

	vmem_entry_from_vaddr(ctx, virt_addr, &entries);

	pte = entries.table[MMU_LEVELS-1] + entries.idx[MMU_LEVELS-1];

	return (mmu_paddr_t)mmu_value(MMU_LEVELS-1, pte) + (virt_addr & MMU_PAGE_MASK);
}

static int vmem_page_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, vmem_page_flags_t flags) {
	struct mmu_entry entries;
	struct mmu_entry *entry = &entries;

	vmem_entry_get_idxs(ctx, virt_addr, &entries);
	vmem_entry_get_tables(ctx, virt_addr, &entries);

	vmem_set_pte_flags(entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL], 0);
	vmem_set_pte_flags(entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL], flags);
	return 0;
}

/**
 * @brief Change PTE attributes for given memory range: [virt_addr, virt_addr + len)
 *
 * @return Negative error value, zero if succeeded
 */
int vmem_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, ssize_t len, vmem_page_flags_t flags) {
	int ret;

	while (len > 0) {
		if ((ret = vmem_page_set_flags(ctx, virt_addr, flags))) {
			return ret;
		}

		virt_addr += MMU_PAGE_SIZE;
		len -= MMU_PAGE_SIZE;
	}

	return 0;
}

void vmem_unmap_region(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size) {
	struct mmu_entry entries;

	for ( ; reg_size; reg_size -= MMU_PAGE_SIZE) {
		vmem_entry_get_idxs(ctx, virt_addr, &entries);
		vmem_entry_get_tables(ctx, virt_addr, &entries);

		vmem_entry_try_free(&entries);
		virt_addr += MMU_PAGE_SIZE;
	}
}
