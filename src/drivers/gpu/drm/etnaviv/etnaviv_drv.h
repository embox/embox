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

#ifndef __ETNAVIV_DRV_H__
#define __ETNAVIV_DRV_H__

#include "etnaviv_compat.h"
#include "etnaviv_drm.h"

struct etnaviv_cmdbuf;
struct etnaviv_gpu;
struct etnaviv_mmu;
struct etnaviv_gem_object;
struct etnaviv_gem_submit;

struct etnaviv_drm_private {
	int num_gpus;
	struct etnaviv_gpu *gpu[ETNA_MAX_PIPES];
};

int etnaviv_ioctl_gem_submit(struct drm_device *dev, void *data,
		struct drm_file *file);

int etnaviv_gem_mmap_offset(struct drm_gem_object *obj, uint64_t *offset);
int etnaviv_gem_new_handle(struct drm_device *dev, struct drm_file *file,
		uint32_t size, uint32_t flags, uint32_t *handle);
struct drm_gem_object *etnaviv_gem_new(struct drm_device *dev,
		uint32_t size, uint32_t flags);
uint16_t etnaviv_buffer_init(struct etnaviv_gpu *gpu);
uint16_t etnaviv_buffer_config_mmuv2(struct etnaviv_gpu *gpu, uint32_t mtlb_addr, uint32_t safe_addr);
void etnaviv_buffer_queue(struct etnaviv_gpu *gpu, unsigned int event,
	struct etnaviv_cmdbuf *cmdbuf);
bool etnaviv_cmd_validate_one(struct etnaviv_gpu *gpu,
	uint32_t *stream, unsigned int size,
	struct drm_etnaviv_gem_submit_reloc *relocs, unsigned int reloc_size);

/*
 * Return the storage size of a structure with a variable length array.
 * The array is nelem elements of elem_size, where the base structure
 * is defined by base.  If the size overflows size_t, return zero.
 */
static inline size_t size_vstruct(size_t nelem, size_t elem_size, size_t base) {
	if (elem_size && nelem > (SIZE_MAX - base) / elem_size)
		return 0;
	return base + nelem * elem_size;
}

/* returns true if fence a comes after fence b */
static inline bool fence_after(uint32_t a, uint32_t b) {
	return (int)(a - b) > 0;
}

static inline bool fence_after_eq(uint32_t a, uint32_t b) {
	return (int)(a - b) >= 0;
}

static inline unsigned long etnaviv_timeout_to_jiffies(
	const struct timespec *timeout) {
	return 0;
}

#endif /* __ETNAVIV_DRV_H__ */
