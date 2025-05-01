/**
 * @file
 * @brief
 *
 * @date 28.09.2012
 * @author Anton Bulychev
 */

#ifndef MEM_VMEM_H_
#define MEM_VMEM_H_

#include <stdint.h>
#include <hal/mmu.h>
#include <stddef.h>
#include <sys/mman.h>

struct mmu_entry {
	uintptr_t entries[MMU_LEVELS];
	uintptr_t *table[MMU_LEVELS];
	int idx[MMU_LEVELS];
};

struct mmu_translate_info {
	struct mmu_entry mmu_entry;
	mmu_ctx_t ctx;
	uintptr_t pte;
};

#define VMEM_PAGE_SIZE        MMU_PAGE_SIZE
#define VMEM_PAGE_MASK        MMU_PAGE_MASK

#define VMEM_PAGE_USERMODE    (1u << 6)

extern int vmem_create_context(mmu_ctx_t *ctx);
extern mmu_ctx_t vmem_current_context(void);
extern void vmem_free_context(mmu_ctx_t ctx);

extern mmu_paddr_t vmem_translate(mmu_ctx_t ctx, mmu_vaddr_t virt_addr,
		struct mmu_translate_info * mmu_translate_info);

extern int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr,
		size_t reg_size, int flags);

extern void vmem_unmap_region(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size);

extern int vmem_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, ssize_t len, int flags);

#define MMU_LAST_LEVEL (MMU_LEVELS - 1)

#ifndef __MMU_SHIFT_1
# if MMU_LEVELS > 1
#  error "MMU_LEVELS does not match __MMU_SHIFT_1"
# else
#  define __MMU_SHIFT_1 0u
# endif
#endif /* __MMU_SHIFT_1 */

#ifndef __MMU_SHIFT_2
# if MMU_LEVELS > 2
#  error "MMU_LEVELS does not match __MMU_SHIFT_2"
# else
#  define __MMU_SHIFT_2 0u
# endif
#endif /* __MMU_SHIFT_2 */

#ifndef __MMU_SHIFT_3
# if MMU_LEVELS > 3
#  error "MMU_LEVELS does not match __MMU_SHIFT_3"
# else
#  define __MMU_SHIFT_3 0u
# endif
#endif /* __MMU_SHIFT_3 */

#ifndef MMU_VADDR_WIDTH
#define MMU_VADDR_WIDTH (8u * sizeof(uintptr_t))
#endif

#define MMU_SHIFT(i) ((i == -1) ? MMU_VADDR_WIDTH : \
			(i) == 0 ? __MMU_SHIFT_0 : \
			(i) == 1 ? __MMU_SHIFT_1 : \
			(i) == 2 ? __MMU_SHIFT_2 : __MMU_SHIFT_3)

#define MMU_ENTRIES(i) (1ul << (MMU_SHIFT(i - 1) - MMU_SHIFT((i))))
#define MMU_MASK(i) ((MMU_ENTRIES(i) - 1) << MMU_SHIFT(i))
#define MMU_SIZE(i) (MMU_ENTRIES(i + 1) * sizeof(mmu_vaddr_t))

#define MMU_PAGE_SIZE  (1u << MMU_SHIFT(MMU_LAST_LEVEL))
#define MMU_PAGE_MASK  (MMU_PAGE_SIZE - 1)

#endif /* MEM_VMEM_H_ */
