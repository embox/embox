/**
 * @file
 * @brief
 *
 * @date 28.09.2012
 * @author Anton Bulychev
 */

#ifndef MEM_VMEM_H_
#define MEM_VMEM_H_

#include <hal/mmu.h>
#include <stddef.h>
#include <sys/mman.h>

#define VMEM_PAGE_SIZE        MMU_PAGE_SIZE
#define VMEM_PAGE_MASK        MMU_PAGE_MASK

#define VMEM_PAGE_WRITABLE    (1 << 0)
#define VMEM_PAGE_CACHEABLE   (1 << 1)
#define VMEM_PAGE_USERMODE    (1 << 2)
/* Used on sparc LEON3 */
#define VMEM_PAGE_EXECUTABLE  (1 << 3)

typedef uint32_t vmem_page_flags_t;

static inline vmem_page_flags_t prot_to_vmem_flags(uint32_t flags) {
	vmem_page_flags_t vmem_page_flags = 0;
	if (flags & PROT_WRITE) {
		vmem_page_flags |= VMEM_PAGE_WRITABLE;
	}
	if (flags & PROT_EXEC) {
		vmem_page_flags |= VMEM_PAGE_EXECUTABLE;
	}
	if (!(flags & PROT_NOCACHE)) {
		vmem_page_flags |= VMEM_PAGE_CACHEABLE;
	}
	return vmem_page_flags;
}

#define vmem_set_context(ctx)   mmu_set_context(ctx)

extern int vmem_create_context(mmu_ctx_t *ctx);
extern mmu_ctx_t vmem_current_context(void);
extern void vmem_free_context(mmu_ctx_t ctx);

extern mmu_paddr_t vmem_translate(mmu_ctx_t ctx, mmu_vaddr_t virt_addr);

extern int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr,
		size_t reg_size, vmem_page_flags_t flags);

extern void vmem_unmap_region(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size);

extern int vmem_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, ssize_t len, vmem_page_flags_t flags);

extern void vmem_on(void);
extern void vmem_off(void);
extern int vmem_mmu_enabled(void);

#define MMU_LAST_LEVEL (MMU_LEVELS - 1)

#ifndef __MMU_SHIFT_1
#define __MMU_SHIFT_1 0
#endif

#ifndef __MMU_SHIFT_2
#define __MMU_SHIFT_2 0
#endif

#ifndef __MMU_SHIFT_3
#define __MMU_SHIFT_3 0
#endif

#define MMU_SHIFT(i) ((i == -1) ? 32 : \
			(i) == 0 ? __MMU_SHIFT_0 : \
			(i) == 1 ? __MMU_SHIFT_1 : \
			(i) == 2 ? __MMU_SHIFT_2 : __MMU_SHIFT_3)

#define MMU_ENTRIES(i) (1 << (MMU_SHIFT(i - 1) - MMU_SHIFT((i))))
#define MMU_MASK(i) ((MMU_ENTRIES(i) - 1) << MMU_SHIFT(i))
#define MMU_SIZE(i) (MMU_ENTRIES(i) * sizeof(mmu_vaddr_t))

#endif /* MEM_VMEM_H_ */
