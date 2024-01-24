/*
 * Copyright (C) 2017 Etnaviv Project
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

#include <stdio.h>
#include <sys/mman.h>

#include <hal/cache.h>
#include <kernel/thread/sync/mutex.h>
#include <util/binalign.h>
#include <lib/libds/bitmap.h>
#include <mem/vmem.h>

#include "etnaviv_compat.h"
#include "etnaviv_cmdbuf.h"
#include "etnaviv_gpu.h"
#include "etnaviv_gem.h"
#include "etnaviv_drv.h"

#define SUBALLOC_SIZE		(256 * 1024)
#define SUBALLOC_GRANULE	(4096)
#define SUBALLOC_GRANULES	(SUBALLOC_SIZE / SUBALLOC_GRANULE)

struct etnaviv_cmdbuf_suballoc {
	struct etnaviv_gpu *gpu;
	void *addr;

	/* GPU mapping */
	uint32_t iova;

	/* allocation management */
	struct mutex lock;
	BITMAP_DECL(bitmap, SUBALLOC_GRANULES);
};

struct etnaviv_cmdbuf_suballoc *
etnaviv_cmdbuf_suballoc_new(struct etnaviv_gpu * gpu) {
	struct etnaviv_cmdbuf_suballoc *suballoc;
	int ret;

	assert(gpu);

	suballoc = sysmalloc(sizeof(struct etnaviv_cmdbuf_suballoc));
	if (!suballoc) {
		log_error("Failed to alloc suballoc structure");
		return ERR_PTR(-ENOMEM);
	}

	memset(suballoc, 0, sizeof(*suballoc));

	suballoc->gpu = gpu;
	suballoc->addr = sysmemalign(0x10000, SUBALLOC_SIZE);

	if (!suballoc->addr) {
		log_error("Failed to allocate suballoc buffer");
		goto free_suballoc;
	}

	if (vmem_set_flags(vmem_current_context(),
					(mmu_vaddr_t) suballoc->addr,
					SUBALLOC_SIZE,
					PROT_WRITE | PROT_READ | PROT_NOCACHE)) {
		log_error("Failed to set vmem flags");
		goto free_dma;
	}
	mmu_flush_tlb();
	dcache_flush(suballoc->addr, SUBALLOC_SIZE);

	memset(suballoc->addr, 0, SUBALLOC_SIZE);

	mutex_init(&suballoc->lock);

	ret = etnaviv_iommu_get_suballoc_va(gpu, (dma_addr_t) suballoc->addr,
					     SUBALLOC_SIZE,
					    &suballoc->iova);
	if (ret) {
		log_error("Failed to get suballoc VA");
		goto free_dma;
	}

	return suballoc;

free_dma:
	sysfree(suballoc->addr);
free_suballoc:
	sysfree(suballoc);

	return NULL;
}

void etnaviv_cmdbuf_suballoc_destroy(struct etnaviv_cmdbuf_suballoc *suballoc) {
	kfree(suballoc->addr);
	kfree(suballoc);
}

struct etnaviv_cmdbuf *etnaviv_cmdbuf_new(
		struct etnaviv_cmdbuf_suballoc *suballoc, uint32_t size,
		size_t nr_bos) {
	struct etnaviv_cmdbuf *cmdbuf;
	int cmdbuf_size = (sizeof(struct etnaviv_cmdbuf)) +
		(sizeof(struct etnaviv_vram_mapping)) * nr_bos;
	int off, granules;

	assert(suballoc);

	mutex_lock(&suballoc->lock);

	granules = binalign_bound(size, SUBALLOC_GRANULE) / SUBALLOC_GRANULE;
	off = bitmap_find_zero_bit(suballoc->bitmap, granules, 0);

	if (off == SUBALLOC_GRANULES) {
		log_error("Suballoc bitmap is full");
		return NULL;
	}

	if (!(cmdbuf = sysmalloc(cmdbuf_size))) {
		log_error("Failed to allocate cmdbuf");
		return NULL;
	}

	*cmdbuf = (struct etnaviv_cmdbuf) {
		.suballoc = suballoc,
		.size = size,
		.suballoc_offset = off * SUBALLOC_GRANULE,
		.vaddr = suballoc->addr + off * SUBALLOC_GRANULE,
	};

	log_debug("Cmdbuf info: addr %p suballoc offt 0x%x size 0x%x",
			cmdbuf, cmdbuf->suballoc_offset, size);

	for (int i = 0; i < granules; i++) {
		bitmap_set_bit(suballoc->bitmap, off + i);
	}

	mutex_unlock(&suballoc->lock);

	return cmdbuf;
}

void etnaviv_cmdbuf_free(struct etnaviv_cmdbuf *cmdbuf) {
	struct etnaviv_cmdbuf_suballoc *suballoc;
	int off, granules;

	assert(cmdbuf);
	assert(cmdbuf->suballoc);

	suballoc = cmdbuf->suballoc;

	mutex_lock(&suballoc->lock);
	off = cmdbuf->suballoc_offset / SUBALLOC_GRANULE;
	granules = cmdbuf->size / SUBALLOC_GRANULE;
	for (int i = 0; i < granules; i++) {
		bitmap_clear_bit(suballoc->bitmap, off + i);
	}
	mutex_unlock(&suballoc->lock);

	sysfree(cmdbuf);
}

uint32_t etnaviv_cmdbuf_get_va(struct etnaviv_cmdbuf *buf) {
	assert(buf);

	log_debug("suballoc %p iova %p offset %p",
			buf->suballoc,
			(void*) buf->suballoc->iova,
			(void*) buf->suballoc_offset);
	return ((uint32_t) buf->vaddr) - 0x10000000;
}

dma_addr_t etnaviv_cmdbuf_get_pa(struct etnaviv_cmdbuf *buf) {
	assert(buf);
	assert(buf->suballoc);
	return (dma_addr_t) (buf->suballoc->addr + buf->suballoc_offset);
}

