/*
 * Copyright (C) 2015 Etnaviv Project
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ETNAVIV_MMU_H__
#define __ETNAVIV_MMU_H__

#include "etnaviv_compat.h"

enum etnaviv_iommu_version {
	ETNAVIV_IOMMU_V1 = 0,
	ETNAVIV_IOMMU_V2,
};

struct etnaviv_gpu;
struct etnaviv_vram_mapping;
struct iommu_domain;

struct iommu_ops {
	void (*domain_free)(struct iommu_domain *domain);
	int (*map)(struct iommu_domain *domain, unsigned long iova, phys_addr_t paddr, size_t size, int prot);
	size_t (*unmap)(struct iommu_domain *domain, unsigned long iova, size_t size);
	phys_addr_t (*iova_to_phys)(struct iommu_domain *domain, dma_addr_t iova);
	int pgsize_bitmap;
};

struct etnaviv_iommu_ops {
	struct iommu_ops ops;
	size_t (*dump_size)(struct iommu_domain *);
	void (*dump)(struct iommu_domain *, void *);
};

struct etnaviv_iommuv2_domain {
	struct device *dev;
	void *bad_page_cpu;
	uint32_t bad_page_dma;
	/* M(aster) TLB aka first level pagetable */
	uint32_t *mtlb_cpu;
	uint32_t mtlb_dma;
	/* S(lave) TLB aka second level pagetable */
	uint32_t *stlb_cpu[1024];
	uint32_t stlb_dma[1024];
};

struct etnaviv_iommu {
	uint8_t domain[sizeof(struct etnaviv_iommuv2_domain)];
	uint32_t start_addr, end_addr, flags; /* iommu_domain */

	enum etnaviv_iommu_version version;

	struct list_head mappings;
	uint32_t last_iova;
	bool need_flush;
};

struct etnaviv_gem_object;

int etnaviv_iommu_get_suballoc_va(struct etnaviv_gpu *gpu, dma_addr_t paddr,
		size_t size, uint32_t *iova);

int etnaviv_iommu_init(struct etnaviv_gpu *gpu);
void etnaviv_iommu_restore(struct etnaviv_gpu *gpu);

#endif /* __ETNAVIV_MMU_H__ */
