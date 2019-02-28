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

#include <etnaviv_xml/common.xml.h>

#include "etnaviv_cmdbuf.h"
#include "etnaviv_drv.h"
#include "etnaviv_gem.h"
#include "etnaviv_gpu.h"
#include "etnaviv_iommu.h"
#include "etnaviv_mmu.h"

int etnaviv_iommu_init(struct etnaviv_gpu *gpu) {
	struct etnaviv_iommu *mmu = &gpu->mmu;

	memset(mmu, 0, sizeof(*mmu));

	if (!(gpu->identity.minor_features1 & chipMinorFeatures1_MMU_VERSION)) {
		etnaviv_iommuv1_domain_init(gpu);
		mmu->version = ETNAVIV_IOMMU_V1;
	} else {
		etnaviv_iommuv2_domain_init(gpu);
		mmu->version = ETNAVIV_IOMMU_V2;
	}

	return 0;
}

void etnaviv_iommu_restore(struct etnaviv_gpu *gpu) {
	if (gpu->mmu.version == ETNAVIV_IOMMU_V1) {
		etnaviv_iommuv1_restore(gpu);
	} else {
		etnaviv_iommuv2_restore(gpu);
	}
}

extern struct etnaviv_iommu_ops etnaviv_iommu_ops;
int etnaviv_iommu_get_suballoc_va(struct etnaviv_gpu *gpu, dma_addr_t paddr,
				size_t size,
				  uint32_t *iova) {
	struct etnaviv_iommu *mmu = &gpu->mmu;

	if (mmu->version == ETNAVIV_IOMMU_V1) {
		*iova = paddr - gpu->memory_base;
		return 0;
	} else {
		int ret;

		ret = etnaviv_iommu_ops.ops.map((void*)mmu->domain, mmu->last_iova, paddr, size, IOMMU_READ);
		if (ret < 0) {
			return ret;
		}
		*iova = mmu->last_iova;
		mmu->last_iova = mmu->last_iova + size;
		mmu->need_flush = true;

		return 0;
	}
}
