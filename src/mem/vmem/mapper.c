/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 * @author Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <hal/mmu.h>
#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>
#include <util/binalign.h>
#include <util/log.h>

static struct mmu_entry *vmem_entry_get_idxs(mmu_ctx_t ctx,
    mmu_vaddr_t virt_addr, struct mmu_entry *entry) {
	int i;

	for (i = 0; i < MMU_LEVELS; i++) {
		entry->idx[i] = (virt_addr & MMU_MASK(i)) >> MMU_SHIFT(i);
	}
	return entry;
}

static struct mmu_entry *vmem_entry_get_tables(mmu_ctx_t ctx,
    mmu_vaddr_t virt_addr, struct mmu_entry *entry) {
	int i;

	entry->table[0] = mmu_get_root(ctx);
	entry->entries[0] = *(entry->table[0] + entry->idx[0]);
	for (i = 1; i < MMU_LEVELS; i++) {
		entry->table[i] = mmu_get(i - 1,
		    entry->table[i - 1] + entry->idx[i - 1]);
		if (entry->table[i] == NULL) {
			break;
		}
		entry->entries[i] = *(entry->table[i] + entry->idx[i]);
	}

	return entry;
}

static struct mmu_entry *vmem_entry_from_vaddr(mmu_ctx_t ctx,
    mmu_vaddr_t virt_addr, struct mmu_entry *entry) {
	vmem_entry_get_idxs(ctx, virt_addr, entry);
	vmem_entry_get_tables(ctx, virt_addr, entry);

	return entry;
}

static int vmem_entry_set_pte(struct mmu_entry *entry, mmu_paddr_t phy_addr,
    int flags) {
	uintptr_t pte;
	int i;

	assert(entry);

	for (i = 0; i < MMU_LAST_LEVEL; i++) {
		if (!mmu_present(i, entry->table[i] + entry->idx[i])) {
			if (!(entry->table[i + 1] = vmem_alloc_table(i + 1))) {
				log_error("Failed to alloc table lvl%d\n", i);
			}

			mmu_set(i, entry->table[i] + entry->idx[i],
			    (uintptr_t)entry->table[i + 1]);
		}
	}
	pte = mmu_pte_pack(phy_addr, flags);
	mmu_pte_set(entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL], pte);

	return 0;
}

static int vmem_entry_try_free(struct mmu_entry *entry) {
	if (entry->table[MMU_LAST_LEVEL]) {
		mmu_unset(MMU_LAST_LEVEL,
		    entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL]);
	}

	return 0;
}

int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr,
    size_t reg_size, int flags) {
	struct mmu_entry entries;

	for (; reg_size; reg_size -= MMU_PAGE_SIZE) {
		vmem_entry_get_idxs(ctx, virt_addr, &entries);
		vmem_entry_get_tables(ctx, virt_addr, &entries);

		vmem_entry_set_pte(&entries, phy_addr, flags);
		phy_addr += MMU_PAGE_SIZE;
		virt_addr += MMU_PAGE_SIZE;
	}

	mmu_flush_tlb();

	return 0;
}

mmu_paddr_t vmem_translate(mmu_ctx_t ctx, mmu_vaddr_t virt_addr,
    struct mmu_translate_info *mmu_translate_info) {
	uintptr_t *pte;
	struct mmu_entry entries;

	vmem_entry_from_vaddr(ctx, virt_addr, &entries);

	pte = entries.table[MMU_LEVELS - 1] + entries.idx[MMU_LEVELS - 1];

	if (mmu_translate_info) {
		memcpy(&mmu_translate_info->mmu_entry, &entries,
		    sizeof(mmu_translate_info->mmu_entry));
		mmu_translate_info->ctx = ctx;
		mmu_translate_info->pte = mmu_pte_get(pte);
	}

	return (mmu_paddr_t)mmu_get(MMU_LEVELS - 1, pte)
	       + (virt_addr & MMU_PAGE_MASK);
}

static int vmem_page_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr,
    int flags) {
	struct mmu_entry entries;
	uintptr_t pte;
	uintptr_t phy_addr;
	int old_flags;
	struct mmu_entry *entry = &entries;

	vmem_entry_get_idxs(ctx, virt_addr, &entries);
	vmem_entry_get_tables(ctx, virt_addr, &entries);

	pte = mmu_pte_get(
	    entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL]);
	phy_addr = mmu_pte_unpack(pte, &old_flags);
	pte = mmu_pte_pack(phy_addr, flags);
	mmu_pte_set(entry->table[MMU_LAST_LEVEL] + entry->idx[MMU_LAST_LEVEL], pte);

	mmu_flush_tlb();

	return 0;
}

/**
 * @brief Change PTE attributes for given memory range: [virt_addr, virt_addr + len)
 *
 * @return Negative error value, zero if succeeded
 */
int vmem_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, ssize_t len,
    int flags) {
	int ret;

	while (len > 0) {
		if ((ret = vmem_page_set_flags(ctx, virt_addr, flags))) {
			return ret;
		}

		virt_addr += MMU_PAGE_SIZE;
		len -= MMU_PAGE_SIZE;
	}

	mmu_flush_tlb();

	return 0;
}

void vmem_unmap_region(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size) {
	struct mmu_entry entries;

	for (; reg_size; reg_size -= MMU_PAGE_SIZE) {
		vmem_entry_get_idxs(ctx, virt_addr, &entries);
		vmem_entry_get_tables(ctx, virt_addr, &entries);

		vmem_entry_try_free(&entries);
		virt_addr += MMU_PAGE_SIZE;
	}

	mmu_flush_tlb();
}
