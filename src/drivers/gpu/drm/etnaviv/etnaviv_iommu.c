/**
 * @file etnaviv_iommu.c
 * @brief
 * @version
 * @date 25.03.2019
 */

#include <stdlib.h>

#include <asm-generic/dma-mapping.h>

#include "etnaviv_gpu.h"
#include "etnaviv_mmu.h"
#include "etnaviv_compat.h"

#include <etnaviv_xml/state_hi.xml.h>

#define PT_SIZE		SZ_2M
#define PT_ENTRIES	(PT_SIZE / sizeof(uint32_t))

#define GPU_MEM_START	0x80000000

struct etnaviv_iommu_domain_pgtable {
	uint32_t *pgtable;
	dma_addr_t paddr;
};

struct etnaviv_iommu_domain {
	struct device *dev;
	void *bad_page_cpu;
	dma_addr_t bad_page_dma;
	struct iommu_ops *ops;
	struct etnaviv_iommu_domain_pgtable pgtable;
};

static struct etnaviv_iommu_domain *to_etnaviv_domain(void *domain) {
	return domain;
}

static int pgtable_alloc(struct etnaviv_iommu_domain_pgtable *pgtable,
			 size_t size) {
	pgtable->pgtable = malloc(size);
	if (!pgtable->pgtable)
		return -ENOMEM;

	pgtable->paddr = (dma_addr_t) pgtable->pgtable;

	return 0;
}

static uint32_t pgtable_read(struct etnaviv_iommu_domain_pgtable *pgtable,
			   unsigned long iova) {
	/* calcuate index into page table */
	unsigned int index = (iova - GPU_MEM_START) / SZ_4K;
	phys_addr_t paddr;

	paddr = pgtable->pgtable[index];

	return paddr;
}

static void pgtable_write(struct etnaviv_iommu_domain_pgtable *pgtable,
			  unsigned long iova, phys_addr_t paddr) {
	/* calcuate index into page table */
	unsigned int index = (iova - GPU_MEM_START) / SZ_4K;

	pgtable->pgtable[index] = paddr;
}

static int __etnaviv_iommu_init(struct etnaviv_iommu_domain *etnaviv_domain) {
	uint32_t *p;
	int ret, i;

	etnaviv_domain->bad_page_cpu = malloc(SZ_4K);

	if (!etnaviv_domain->bad_page_cpu)
		return -ENOMEM;

	p = etnaviv_domain->bad_page_cpu;
	for (i = 0; i < SZ_4K / 4; i++)
		*p++ = 0xdead55aa;

	ret = pgtable_alloc(&etnaviv_domain->pgtable, PT_SIZE);
	if (ret < 0) {
		free(etnaviv_domain->bad_page_cpu);
		return ret;
	}

	for (i = 0; i < PT_ENTRIES; i++)
		etnaviv_domain->pgtable.pgtable[i] =
			etnaviv_domain->bad_page_dma;

	return 0;
}

static void etnaviv_domain_free(struct iommu_domain *domain) {
	struct etnaviv_iommu_domain *etnaviv_domain = to_etnaviv_domain(domain);

	free(etnaviv_domain->pgtable.pgtable);

	free(etnaviv_domain->bad_page_cpu);

	kfree(etnaviv_domain);
}

static int etnaviv_iommuv1_map(struct iommu_domain *domain, unsigned long iova,
	   phys_addr_t paddr, size_t size, int prot) {
	struct etnaviv_iommu_domain *etnaviv_domain = to_etnaviv_domain(domain);

	if (size != SZ_4K)
		return -EINVAL;

	pgtable_write(&etnaviv_domain->pgtable, iova, paddr);

	return 0;
}

static size_t etnaviv_iommuv1_unmap(struct iommu_domain *domain,
	unsigned long iova, size_t size) {
	struct etnaviv_iommu_domain *etnaviv_domain = to_etnaviv_domain(domain);

	if (size != SZ_4K)
		return -EINVAL;

	pgtable_write(&etnaviv_domain->pgtable, iova,
		      etnaviv_domain->bad_page_dma);

	return SZ_4K;
}

static phys_addr_t etnaviv_iommu_iova_to_phys(struct iommu_domain *domain,
	dma_addr_t iova) {
	struct etnaviv_iommu_domain *etnaviv_domain = to_etnaviv_domain(domain);

	return pgtable_read(&etnaviv_domain->pgtable, iova);
}

static size_t etnaviv_iommuv1_dump_size(struct iommu_domain *domain) {
	return PT_SIZE;
}

static void etnaviv_iommuv1_dump(struct iommu_domain *domain, void *buf) {
	struct etnaviv_iommu_domain *etnaviv_domain = to_etnaviv_domain(domain);

	memcpy(buf, etnaviv_domain->pgtable.pgtable, PT_SIZE);
}

struct etnaviv_iommu_ops etnaviv_iommu_ops = {
	.ops = {
		.domain_free = etnaviv_domain_free,
		.map = etnaviv_iommuv1_map,
		.unmap = etnaviv_iommuv1_unmap,
		.iova_to_phys = etnaviv_iommu_iova_to_phys,
		.pgsize_bitmap = SZ_4K,
	},
	.dump_size = etnaviv_iommuv1_dump_size,
	.dump = etnaviv_iommuv1_dump,
};

void etnaviv_iommuv1_restore(struct etnaviv_gpu *gpu) {
	struct etnaviv_iommu_domain *etnaviv_domain =
			to_etnaviv_domain(gpu->mmu.domain);
	uint32_t pgtable;

	/* set base addresses */
	gpu_write(gpu, VIVS_MC_MEMORY_BASE_ADDR_RA, gpu->memory_base);
	gpu_write(gpu, VIVS_MC_MEMORY_BASE_ADDR_FE, gpu->memory_base);
	gpu_write(gpu, VIVS_MC_MEMORY_BASE_ADDR_TX, gpu->memory_base);
	gpu_write(gpu, VIVS_MC_MEMORY_BASE_ADDR_PEZ, gpu->memory_base);
	gpu_write(gpu, VIVS_MC_MEMORY_BASE_ADDR_PE, gpu->memory_base);

	/* set page table address in MC */
	pgtable = (uint32_t) etnaviv_domain->pgtable.paddr;

	gpu_write(gpu, VIVS_MC_MMU_FE_PAGE_TABLE, pgtable);
	gpu_write(gpu, VIVS_MC_MMU_TX_PAGE_TABLE, pgtable);
	gpu_write(gpu, VIVS_MC_MMU_PE_PAGE_TABLE, pgtable);
	gpu_write(gpu, VIVS_MC_MMU_PEZ_PAGE_TABLE, pgtable);
	gpu_write(gpu, VIVS_MC_MMU_RA_PAGE_TABLE, pgtable);
}

int etnaviv_iommuv1_domain_init(struct etnaviv_gpu *gpu) {
	struct etnaviv_iommu_domain *etnaviv_domain;
	int ret;

	etnaviv_domain = (void *) &gpu->mmu.domain;

	memset(etnaviv_domain, 0, sizeof(*etnaviv_domain));

	etnaviv_domain->ops = &etnaviv_iommu_ops.ops;
	gpu->mmu.start_addr = GPU_MEM_START;
	gpu->mmu.end_addr = GPU_MEM_START + PT_ENTRIES * SZ_4K - 1;

	if ((ret = __etnaviv_iommu_init(etnaviv_domain))) {
		return ret;
	}

	return 0;
}
