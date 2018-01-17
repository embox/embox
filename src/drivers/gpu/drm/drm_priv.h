/**
 * @file
 *
 * @date Jan 18, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPU_DRM_DRM_PRIV_H_
#define SRC_DRIVERS_GPU_DRM_DRM_PRIV_H_

#include <pthread.h>

#include <linux/idr.h>

struct reservation_object;

struct drm_device  {
	void *dev_private;
	struct device *dev;		/**< Device structure of bus-device */
};

struct drm_file {
	struct idr object_idr;
	pthread_mutex_t table_lock;

	void *driver_priv;
};


#endif /* SRC_DRIVERS_GPU_DRM_DRM_PRIV_H_ */
