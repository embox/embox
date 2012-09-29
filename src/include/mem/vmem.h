/**
 * @file
 * @brief
 *
 * @date 28.09.2012
 * @author Anton Bulychev
 */

#ifndef MEM_VMEM_H_
#define MEM_VMEM_H_

#include <hal/mm/mmu_core.h>

struct task_vmem_data {
	mmu_ctx_t ctx;
};

extern void vmem_create_virtual_space(mmu_ctx_t ctx);

#endif /* MEM_VMEM_H_ */
