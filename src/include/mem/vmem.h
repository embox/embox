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

#define vmem_set_context(ctx)   mmu_set_context(ctx)

extern int vmem_create_context(mmu_ctx_t *ctx);
extern int vmem_init_context(mmu_ctx_t *ctx);
extern mmu_ctx_t vmem_current_context(void);
extern void vmem_free_context(mmu_ctx_t ctx);
extern mmu_paddr_t vmem_translate(mmu_ctx_t ctx, mmu_vaddr_t virt_addr);
extern int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr, size_t reg_size, vmem_page_flags_t flags);
extern int vmem_create_space(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size, vmem_page_flags_t flags);
extern int vmem_page_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, vmem_page_flags_t flags);
extern void vmem_unmap_region(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size, int free_pages);
extern int vmem_copy_region(mmu_ctx_t nctx, mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size);
extern void vmem_handle_page_fault(mmu_vaddr_t virt_addr);

extern void vmem_on(void);
extern void vmem_off(void);
extern int vmem_mmu_enabled(void);

#define EXTRACT_IDX_FROM_VADDR(virt_addr, pgd_idx, pmd_idx, pte_idx) \
	pgd_idx = ((uint32_t) virt_addr & MMU_PGD_MASK) >> MMU_PGD_SHIFT; \
	pmd_idx = ((uint32_t) virt_addr & MMU_PMD_MASK) >> MMU_PMD_SHIFT; \
	pte_idx = ((uint32_t) virt_addr & MMU_PTE_MASK) >> MMU_PTE_SHIFT

#if 0
static inline int vmem_map_on_itself(mmu_ctx_t ctx, void *addr, size_t size, vmem_page_flags_t flags) {
	/* Considering that address was aligned, but size may be not */
	size = (size + MMU_PAGE_MASK) & (~MMU_PAGE_MASK);
	return vmem_map_region(ctx, (mmu_paddr_t) addr, (mmu_vaddr_t) addr, (size_t) size, flags);
}
#endif

#endif /* MEM_VMEM_H_ */
