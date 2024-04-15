/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#ifndef MEM_MMAP_PRIV_H_
#define MEM_MMAP_PRIV_H_

#include <stdint.h>
#include <lib/libds/dlist.h>
#include <hal/mmu.h>

struct phy_page {
	void *page;
	size_t page_number;

	struct dlist_head page_link;
};

struct emmap {
	void *brk;

	mmu_ctx_t ctx;

	struct dlist_head marea_list;
	struct dlist_head page_list;
};

extern int mmap_place(struct emmap *mmap, uintptr_t start, size_t size, int flags);
extern int mmap_release(struct emmap *mmap, uintptr_t addr);
extern uintptr_t mmap_alloc(struct emmap *mmap, size_t size);
extern int mmap_prot(struct emmap *mmap, uintptr_t addr);

extern void mmap_add_phy_page(struct emmap *mmap, struct phy_page *phy_page);
extern void mmap_del_phy_page(struct phy_page *phy_page);
extern struct phy_page *mmap_find_phy_page(struct emmap *mmap, void *start);

#endif /* MEM_MMAP_PRIV_H_ */
