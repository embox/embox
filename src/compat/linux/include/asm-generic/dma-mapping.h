/**
 * @file dma-mapping.h
 * @brief Allocate/free DMA memory
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.02.2018
 */

#ifndef LINUX_DMA_MAPPING_H_
#define LINUX_DMA_MAPPING_H_

#include <linux/types.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <assert.h>

#include <mem/sysmalloc.h>
#include <mem/vmem.h>

struct device;

/**
 * Allocate strong-ordered memory
 */
static inline void *dma_alloc_coherent(struct device *dev, size_t size,
		dma_addr_t *handle, gfp_t flag) {
	void *mem = sysmalloc(size);
	mmu_ctx_t ctx = vmem_current_context();
	int flags = PROT_WRITE | PROT_READ /*| PROT_NOCACHE */;

	vmem_set_flags(ctx, (mmu_vaddr_t) mem, size, flags);

	return mem;
}

static inline void dma_free_coherent(struct device *dev, size_t size,
		void *cpu_addr, dma_addr_t handle) {
	sysfree(cpu_addr);
}

#endif /* LINUX_DMA_MAPPING_H_ */
