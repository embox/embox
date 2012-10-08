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
#include <types.h>

#define VMEM_PAGE_WRITABLE  (1 << 0)
#define VMEM_PAGE_CACHEABLE (1 << 1)
#define VMEM_PAGE_USERMODE  (1 << 2)

typedef uint32_t vmem_page_flags_t;

struct task_vmem_data {
	mmu_ctx_t ctx;
};

extern int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr, size_t reg_size, vmem_page_flags_t flags);
extern void vmem_create_virtual_space(mmu_ctx_t ctx);

#endif /* MEM_VMEM_H_ */
