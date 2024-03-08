/**
 * @file
 *
 * @date Jan 18, 2018
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <stdio.h>
#include <util/err.h>
#include <mem/sysmalloc.h>

#include <embox_drm/drm_priv.h>
#include <embox_drm/drm_gem.h>

#include "etnaviv_gem.h"
#include "etnaviv_drv.h"

int etnaviv_gem_obj_add(struct drm_device *dev, struct drm_gem_object *obj) {
	return 0;
}

static int etnaviv_gem_new_impl(struct drm_device *dev, uint32_t size,
		uint32_t flags, struct reservation_object *robj,
		void *ops, struct drm_gem_object **obj) {
	struct etnaviv_gem_object *etnaviv_obj;
	unsigned sz = sizeof(*etnaviv_obj);
	etnaviv_obj = sysmalloc(sz);

	if (!etnaviv_obj) {
		log_debug("enomem\n");
		return -ENOMEM;
	}

	etnaviv_obj->flags = flags;

	mutex_init(&etnaviv_obj->lock);

	*obj = &etnaviv_obj->base;

	return 0;
}

static struct drm_gem_object *__etnaviv_gem_new(struct drm_device *dev,
		uint32_t size, uint32_t flags) {
	struct drm_gem_object *obj = NULL;
	int ret;

	ret = etnaviv_gem_new_impl(dev, size, flags, NULL, NULL, &obj);
	if (ret) {
		log_debug("err 1\n");
		goto fail;
	}

	ret = drm_gem_object_init(dev, obj, size);
	log_debug("ret %p\n", obj);
	return obj;

fail:
	return err2ptr(ret);
}

/* convenience method to construct a GEM buffer object, and userspace handle */
int etnaviv_gem_new_handle(struct drm_device *dev, struct drm_file *file,
		uint32_t size, uint32_t flags, uint32_t *handle) {
	struct drm_gem_object *obj;
	int ret;

	obj = __etnaviv_gem_new(dev, size, flags);
	if (ptr2err(obj)) {
		return (int) obj;
	}

	ret = etnaviv_gem_obj_add(dev, obj);
	if (ret < 0) {
		return ret;
	}

	return drm_gem_handle_create(file, obj, handle);
}

struct drm_gem_object *etnaviv_gem_new(struct drm_device *dev, uint32_t size,
		uint32_t flags) {
	struct drm_gem_object *obj;
	int ret;

	obj = __etnaviv_gem_new(dev, size, flags);
	if (ptr2err(obj))
		return obj;

	if ((ret = etnaviv_gem_obj_add(dev, obj)) < 0) {
		return err2ptr(ret);
	}

	return obj;
}


int etnaviv_gem_mmap_offset(struct drm_gem_object *obj, uint64_t *offset) {
	*offset = (int) obj;
	return 0;
}

struct etnaviv_vram_mapping *etnaviv_gem_mapping_get(
	struct drm_gem_object *obj, struct etnaviv_gpu *gpu) {
	struct etnaviv_gem_object *etnaviv_obj = to_etnaviv_bo(obj);
	struct etnaviv_vram_mapping *mapping;

	if (!(mapping = kzalloc(sizeof(*mapping), GFP_KERNEL))) {
		return 0;
	}

	mapping->object = etnaviv_obj;
	mapping->iova = (uint32_t) obj->dma_buf - 0x10000000;

	return mapping;
}
