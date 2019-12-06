/**
 * @file
 * @brief GEM Graphics Execution Manager Driver Interfaces
 *
 * @date Jan 19, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPU_DRM_DRM_GEM_H_
#define SRC_DRIVERS_GPU_DRM_DRM_GEM_H_

#include <stdint.h>
#include <stddef.h>

struct file_desc;
struct drm_device;
struct drm_file;

struct drm_gem_object {
	/**
	 * @handle_count:
	 *
	 * This is the GEM file_priv handle count of this object.
	 *
	 * Each handle also holds a reference. Note that when the handle_count
	 * drops to 0 any global names (e.g. the id in the flink namespace) will
	 * be cleared.
	 *
	 * Protected by &drm_device.object_name_lock.
	 */
	unsigned handle_count;

	/**
	 * @dev: DRM dev this object belongs to.
	 */
	struct drm_device *dev;

	/**
	 * @filp:
	 *
	 * SHMEM file node used as backing storage for swappable buffer objects.
	 * GEM also supports driver private objects with driver-specific backing
	 * storage (contiguous CMA memory, special reserved blocks). In this
	 * case @filp is NULL.
	 */
	struct file_desc *filp;

	/**
	 * @size:
	 *
	 * Size of the object, in bytes.  Immutable over the object's
	 * lifetime.
	 */
	size_t size;

	/**
	 * @name:
	 *
	 * Global name for this object, starts at 1. 0 means unnamed.
	 * Access is covered by &drm_device.object_name_lock. This is used by
	 * the GEM_FLINK and GEM_OPEN ioctls.
	 */
	int name;

	void *dma_buf;
};

extern struct drm_gem_object *
drm_gem_object_lookup(struct drm_file *filp, uint32_t handle);

extern int drm_gem_handle_create(struct drm_file *file_priv,
			  struct drm_gem_object *obj,
			  uint32_t *handlep);

extern int drm_gem_object_init(struct drm_device *dev,
		struct drm_gem_object *obj, size_t size);

extern void drm_gem_private_object_init(struct drm_device *dev,
		 struct drm_gem_object *obj, size_t size);

#endif /* SRC_DRIVERS_GPU_DRM_DRM_GEM_H_ */
