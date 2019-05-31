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

#include <stdio.h>

#include <util/log.h>

#include "etnaviv_compat.h"
#include "etnaviv_cmdbuf.h"
#include "etnaviv_drv.h"
#include "etnaviv_gpu.h"
#include "etnaviv_gem.h"
#include "etnaviv_drm.h"

#include <embox_drm/drm_gem.h>

#define BO_INVALID_FLAGS ~(ETNA_SUBMIT_BO_READ | ETNA_SUBMIT_BO_WRITE)
/* make sure these don't conflict w/ ETNAVIV_SUBMIT_BO_x */
#define BO_LOCKED   0x4000
#define BO_PINNED   0x2000

static struct etnaviv_gem_submit *submit_create(struct drm_device *dev,
		struct etnaviv_gpu *gpu, size_t nr) {
	/* In current implementation we process single gem_submit
	 * at any time, so just return static structure instead of malloc */
	static struct etnaviv_gem_submit submit;
	memset(&submit, 0, sizeof(submit));
	submit.dev = dev;
	submit.gpu = gpu;

	return &submit;
}

static int submit_lookup_objects(struct etnaviv_gem_submit *submit,
	struct drm_file *file, struct drm_etnaviv_gem_submit_bo *submit_bos,
	unsigned nr_bos) {
	struct drm_etnaviv_gem_submit_bo *bo;
	unsigned i;
	int ret = 0;

	for (i = 0, bo = submit_bos; i < nr_bos; i++, bo++) {
		struct drm_gem_object *obj;

		if (bo->flags & BO_INVALID_FLAGS) {
			DRM_ERROR("invalid flags: %x\n", bo->flags);
			ret = -EINVAL;
			goto out_unlock;
		}

		submit->bos[i].flags = bo->flags;

		/* normally use drm_gem_object_lookup(), but for bulk lookup
		 * all under single table_lock just hit object_idr directly:
		 */
		obj = idr_find(&file->object_idr, bo->handle);
		if (!obj) {
			DRM_ERROR("invalid handle %u at index %u\n",
				  bo->handle, i);
			ret = -EINVAL;
			goto out_unlock;
		}

		/*
		 * Take a refcount on the object. The file table lock
		 * prevents the object_idr's refcount on this being dropped.
		 */
		submit->bos[i].obj = to_etnaviv_bo(obj);
	}

out_unlock:
	submit->nr_bos = i;

	return ret;
}

static int submit_pin_objects(struct etnaviv_gem_submit *submit) {
	int i, ret = 0;

	for (i = 0; i < submit->nr_bos; i++) {
		struct etnaviv_gem_object *etnaviv_obj = submit->bos[i].obj;
		struct etnaviv_vram_mapping *mapping;

		mapping = etnaviv_gem_mapping_get(&etnaviv_obj->base,
						  submit->gpu);
		log_debug("map for %d is %p\n", i, mapping);
		submit->bos[i].flags |= BO_PINNED;
		submit->bos[i].mapping = mapping;
	}

	return ret;
}

static int submit_bo(struct etnaviv_gem_submit *submit, uint32_t idx,
	struct etnaviv_gem_submit_bo **bo) {
	if (idx >= submit->nr_bos) {
		DRM_ERROR("invalid buffer index: %u (out of %u)\n",
				idx, submit->nr_bos);
		submit->nr_bos = idx;

		return -EINVAL;
	}

	*bo = &submit->bos[idx];

	return 0;
}

/* process the reloc's and patch up the cmdstream as needed: */
static int submit_reloc(struct etnaviv_gem_submit *submit, void *stream,
		uint32_t size, const struct drm_etnaviv_gem_submit_reloc *relocs,
		uint32_t nr_relocs)
{
	uint32_t i, last_offset = 0;
	uint32_t *ptr = stream;
	int ret;

	for (i = 0; i < nr_relocs; i++) {
		const struct drm_etnaviv_gem_submit_reloc *r = relocs + i;
		struct etnaviv_gem_submit_bo *bo;
		uint32_t off;

		if (r->submit_offset % 4) {
			DRM_ERROR("non-aligned reloc offset: %u\n",
				  r->submit_offset);
			return -EINVAL;
		}

		/* offset in dwords: */
		off = r->submit_offset / 4;

		if ((off >= size ) ||
				(off < last_offset)) {
			DRM_ERROR("invalid offset %u at reloc %u\n", off, i);
			return -EINVAL;
		}

		ret = submit_bo(submit, r->reloc_idx, &bo);
		if (ret)
			return ret;

		if (r->reloc_offset >= bo->obj->base.size - sizeof(*ptr)) {
			DRM_ERROR("relocation %u outside object", i);
			return -EINVAL;
		}
		ptr[off] = bo->mapping->iova + (uint32_t) r->reloc_offset;

		last_offset = off;
	}

	return 0;
}

extern void etnaviv_buffer_dump(struct etnaviv_gpu *gpu,
	struct etnaviv_cmdbuf *buf, uint32_t off, uint32_t len);

int etnaviv_ioctl_gem_submit(struct drm_device *dev, void *data, struct drm_file *file) {
	struct etnaviv_drm_private *priv = dev->dev_private;
	struct drm_etnaviv_gem_submit *args = data;
	struct drm_etnaviv_gem_submit_reloc *relocs;
	struct drm_etnaviv_gem_submit_bo *bos;
	struct etnaviv_gem_submit *submit;
	struct etnaviv_cmdbuf *cmdbuf;
	struct etnaviv_gpu *gpu;
	void *stream;
	int ret;

	log_debug("Pipe is %d", args->pipe);
	args->pipe = 0;
	if (args->pipe >= ETNA_MAX_PIPES) {
		return -EINVAL;
	}

	gpu = priv->gpu[args->pipe];
	if (!gpu) {
		return -ENXIO;
	}

	if (args->stream_size % 4) {
		log_error("non-aligned cmdstream buffer size: %u",
			  args->stream_size);
		return -EINVAL;
	}

	if (args->exec_state != ETNA_PIPE_3D &&
	    args->exec_state != ETNA_PIPE_2D &&
	    args->exec_state != ETNA_PIPE_VG) {
		DRM_ERROR("invalid exec_state: 0x%x", args->exec_state);
		return -EINVAL;
	}

	/*
	 * Copy the command submission and bo array to kernel space in
	 * one go, and do this outside of any locks.
	 */
	bos = (void *) (uint32_t) args->bos;
	relocs = (void *) (uint32_t) args->relocs;
	cmdbuf = etnaviv_cmdbuf_new(gpu->cmdbuf_suballoc,
				    ALIGN(args->stream_size, 8) + 8,
				    args->nr_bos);
	if (/* !bos || !relocs || */ !cmdbuf) {
		log_error("smth is null bos %p relocs %p cmdbuf %p", bos, relocs, cmdbuf);
		return -ENOMEM;
	}

	cmdbuf->exec_state = args->exec_state * 0; /* XXX */
	cmdbuf->ctx = file->driver_priv;

	stream = (void *) (int) args->stream;
	submit = submit_create(dev, gpu, args->nr_bos);
	if (!submit) {
		return -ENOMEM;
	}

	submit->flags = 0;//args->flags;

	if (bos) {
		if ((ret = submit_lookup_objects(submit, file, bos, args->nr_bos))) {
			return ret;
		}
	}

	memcpy(cmdbuf->vaddr, stream, args->stream_size);
	cmdbuf->user_size = ALIGN(args->stream_size, 8);

	etnaviv_buffer_dump(gpu, cmdbuf, 0, cmdbuf->user_size);

	if (!etnaviv_cmd_validate_one(gpu, stream, args->stream_size / 4,
				relocs, args->nr_relocs)) {
		return -EINVAL;
	}

	if ((ret = submit_pin_objects(submit))) {
		log_error("submit_pit_objects failed with %d", ret);
		return ret;
	}

	if (relocs && 0) {
		ret = submit_reloc(submit, stream, args->stream_size / 4,
				relocs, args->nr_relocs);
		if (ret) {
			log_error("submit_reloc fail");
			return ret;
		}
	}

	memcpy(cmdbuf->vaddr, stream, args->stream_size);
	cmdbuf->user_size = ALIGN(args->stream_size, 8);

	return etnaviv_gpu_submit(gpu, submit, cmdbuf);
}
