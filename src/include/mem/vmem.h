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

struct task_vmem_data {
	mmu_ctx_t ctx;
};

extern int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr, size_t reg_size, mmu_page_flags_t flags);
extern void vmem_create_virtual_space(mmu_ctx_t ctx);

#endif /* MEM_VMEM_H_ */
