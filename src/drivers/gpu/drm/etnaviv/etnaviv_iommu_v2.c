/*
 * Copyright (C) 2016 Etnaviv Project
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

#include <asm-generic/dma-mapping.h>

#include "etnaviv_compat.h"
#include "etnaviv_cmdbuf.h"
#include "etnaviv_gpu.h"
#include "etnaviv_mmu.h"
#include "etnaviv_drv.h"
#include "etnaviv_iommu.h"

#include <etnaviv_xml/state.xml.h>
#include <etnaviv_xml/state_hi.xml.h>

#define MMUv2_PTE_PRESENT		BIT(0)
#define MMUv2_PTE_EXCEPTION		BIT(1)
#define MMUv2_PTE_WRITEABLE		BIT(2)

#define MMUv2_MTLB_MASK			0xffc00000
#define MMUv2_MTLB_SHIFT		22
#define MMUv2_STLB_MASK			0x003ff000
#define MMUv2_STLB_SHIFT		12

#define MMUv2_MAX_STLB_ENTRIES		1024

static int etnaviv_iommuv2_init(struct etnaviv_gpu *gpu) {
	uint32_t *p;
	int ret, i, j;

	struct etnaviv_iommuv2_domain *etnaviv_domain = (void*) gpu->mmu.domain;
	/* allocate scratch page */
	etnaviv_domain->bad_page_cpu = dma_alloc_coherent(etnaviv_domain->dev,
						  SZ_4K,
						  &etnaviv_domain->bad_page_dma,
						  GFP_KERNEL);
	if (!etnaviv_domain->bad_page_cpu) {
		ret = -ENOMEM;
		goto fail_mem;
	}
	p = etnaviv_domain->bad_page_cpu;
	for (i = 0; i < SZ_4K / 4; i++)
		*p++ = 0xdead55aa;

	etnaviv_domain->mtlb_cpu = dma_alloc_coherent(etnaviv_domain->dev,
						  SZ_4K,
						  &etnaviv_domain->mtlb_dma,
						  GFP_KERNEL);
	if (!etnaviv_domain->mtlb_cpu) {
		ret = -ENOMEM;
		goto fail_mem;
	}

	/* pre-populate STLB pages (may want to switch to on-demand later) */
	for (i = 0; i < MMUv2_MAX_STLB_ENTRIES; i++) {
		etnaviv_domain->stlb_cpu[i] =
				dma_alloc_coherent(etnaviv_domain->dev,
						   SZ_4K,
						   &etnaviv_domain->stlb_dma[i],
						   GFP_KERNEL);
		if (!etnaviv_domain->stlb_cpu[i]) {
			ret = -ENOMEM;
			goto fail_mem;
		}
		p = etnaviv_domain->stlb_cpu[i];
		for (j = 0; j < SZ_4K / 4; j++)
			*p++ = MMUv2_PTE_EXCEPTION;

		etnaviv_domain->mtlb_cpu[i] = etnaviv_domain->stlb_dma[i] |
					      MMUv2_PTE_PRESENT;
	}

	return 0;

fail_mem:
	if (etnaviv_domain->bad_page_cpu)
		dma_free_coherent(etnaviv_domain->dev, SZ_4K,
				  etnaviv_domain->bad_page_cpu,
				  etnaviv_domain->bad_page_dma);

	if (etnaviv_domain->mtlb_cpu)
		dma_free_coherent(etnaviv_domain->dev, SZ_4K,
				  etnaviv_domain->mtlb_cpu,
				  etnaviv_domain->mtlb_dma);

	for (i = 0; i < MMUv2_MAX_STLB_ENTRIES; i++) {
		if (etnaviv_domain->stlb_cpu[i])
			dma_free_coherent(etnaviv_domain->dev, SZ_4K,
					  etnaviv_domain->stlb_cpu[i],
					  etnaviv_domain->stlb_dma[i]);
	}

	return ret;
}

void etnaviv_iommuv2_restore(struct etnaviv_gpu *gpu) {
	struct etnaviv_iommuv2_domain *etnaviv_domain = (void *) gpu->mmu.domain;
	uint16_t prefetch;

	/* If the MMU is already enabled the state is still there. */
	if (gpu_read(gpu, VIVS_MMUv2_CONTROL) & VIVS_MMUv2_CONTROL_ENABLE)
		return;

	prefetch = etnaviv_buffer_config_mmuv2(gpu,
				(uint32_t)etnaviv_domain->mtlb_dma,
				(uint32_t)etnaviv_domain->bad_page_dma);
	etnaviv_gpu_start_fe(gpu, (uint32_t)etnaviv_cmdbuf_get_pa(gpu->buffer),
			     prefetch);
	etnaviv_gpu_wait_idle(gpu, 100);

	gpu_write(gpu, VIVS_MMUv2_CONTROL, VIVS_MMUv2_CONTROL_ENABLE);
}

int etnaviv_iommuv2_domain_init(struct etnaviv_gpu *gpu) {
	gpu->mmu.start_addr = 0;
	gpu->mmu.end_addr = 4096 - 1;

	memset(gpu->mmu.domain, 0, sizeof(struct etnaviv_iommuv2_domain));
	if (etnaviv_iommuv2_init(gpu)) {
		return -1;
	}

	return 0;
}
