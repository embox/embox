/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_NOMMU_H
#define VMEM_NOMMU_H

#include <hal/mmu.h>
#include <sys/mman.h>

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

#define vmem_mmu_enabled() 0

static inline int vmem_create_context(mmu_ctx_t *ctx) {
	(void)ctx;
	return 0;
}

#define vmem_current_context() 0

#define vmem_free_context()

#define vmem_translate(a,b) b

#endif /* VMEM_NOMMU_H */
