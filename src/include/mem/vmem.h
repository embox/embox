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

#define VMEM_PAGE_SIZE        MMU_PAGE_SIZE
#define VMEM_PAGE_MASK        MMU_PAGE_MASK

#define VMEM_PAGE_WRITABLE    (1 << 0)
#define VMEM_PAGE_CACHEABLE   (1 << 1)
#define VMEM_PAGE_USERMODE    (1 << 2)



/* Not implemented yet */
#define VMEM_PAGE_EXECUTABLE  (0)

typedef uint32_t vmem_page_flags_t;


extern int vmem_init_context(mmu_ctx_t *ctx);
extern void vmem_free_context(mmu_ctx_t ctx);
extern mmu_paddr_t vmem_translate(mmu_ctx_t ctx, mmu_vaddr_t virt_addr);
extern int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr, size_t reg_size, vmem_page_flags_t flags);
extern int vmem_create_space(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size, vmem_page_flags_t flags);
extern int vmem_page_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, vmem_page_flags_t flags);
extern void vmem_unmap_region(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size, int free_pages);
extern int vmem_copy_region(mmu_ctx_t nctx, mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size);

#define vmem_on() mmu_on()
#define vmem_set_context(ctx)   mmu_set_context(ctx)

//extern void vmem_create_virtual_space(mmu_ctx_t ctx);

#endif /* MEM_VMEM_H_ */
