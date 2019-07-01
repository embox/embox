/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_NOMMU_H
#define VMEM_NOMMU_H

#include <stdint.h>

#include <hal/mmu.h>
#include <sys/mman.h>

#define VMEM_PAGE_USERMODE    (1 << 6)

#define vmem_mmu_enabled() 0

static inline int vmem_create_context(mmu_ctx_t *ctx) {
	(void)ctx;
	return 0;
}

#define vmem_current_context() 0

#define vmem_free_context()

#define vmem_translate(a,b) b

#endif /* VMEM_NOMMU_H */
