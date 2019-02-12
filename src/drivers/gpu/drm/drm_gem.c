/**
 * @file
 *
 * @date Jan 19, 2018
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>
#include <pthread.h>

#include <linux/idr.h>
#include <linux/pagemap.h>

#include <embox_drm/drm_gem.h>
#include <embox_drm/drm_priv.h>

/**
 * drm_gem_object_lookup - look up a GEM object from it's handle
 * @filp: DRM file private date
 * @handle: userspace handle
 *
 * Returns:
 *
 * A reference to the object named by the handle if such exists on @filp, NULL
 * otherwise.
 */
struct drm_gem_object *
drm_gem_object_lookup(struct drm_file *filp, uint32_t handle) {
	struct drm_gem_object *obj;

	pthread_mutex_lock(&filp->table_lock);

	obj = idr_find(&filp->object_idr, handle);

	pthread_mutex_unlock(&filp->table_lock);

	return obj;
}

/**
 * drm_gem_handle_create_tail - internal functions to create a handle
 * @file_priv: drm file-private structure to register the handle for
 * @obj: object to register
 * @handlep: pointer to return the created handle to the caller
 *
 * This expects the dev->object_name_lock to be held already and will drop it
 * before returning. Used to avoid races in establishing new handles when
 * importing an object from either an flink name or a dma-buf.
 *
 * Handles must be release again through drm_gem_handle_delete(). This is done
 * when userspace closes @file_priv for all attached handles, or through the
 * GEM_CLOSE ioctl for individual handles.
 */
int drm_gem_handle_create_tail(struct drm_file *file_priv,
			   struct drm_gem_object *obj,
			   uint32_t *handlep) {
	int ret;

	obj->handle_count++;

	pthread_mutex_lock(&file_priv->table_lock);

	ret = idr_alloc(&file_priv->object_idr, obj, 1, 0, GFP_NOWAIT);

	if (ret < 0) {
		return ret;
	}

	pthread_mutex_unlock(&file_priv->table_lock);

	*handlep = ret;

	return 0;
}

/**
 * drm_gem_handle_create - create a gem handle for an object
 * @file_priv: drm file-private structure to register the handle for
 * @obj: object to register
 * @handlep: pionter to return the created handle to the caller
 *
 * Create a handle for this object. This adds a handle reference
 * to the object, which includes a regular reference count. Callers
 * will likely want to dereference the object afterwards.
 */
int drm_gem_handle_create(struct drm_file *file_priv,
			  struct drm_gem_object *obj,
			  uint32_t *handlep) {
	return drm_gem_handle_create_tail(file_priv, obj, handlep);
}

/**
 * drm_gem_object_init - initialize an allocated shmem-backed GEM object
 * @dev: drm_device the object should be initialized for
 * @obj: drm_gem_object to initialize
 * @size: object size
 *
 * Initialize an already allocated GEM object of the specified size with
 * shmfs backing store.
 */
int drm_gem_object_init(struct drm_device *dev,
			struct drm_gem_object *obj, size_t size) {
	*obj = (struct drm_gem_object) {
		.dev  = dev,
		.size = size,
	};
	return 0;
}
