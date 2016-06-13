/**
 * @file
 *
 * @date 13.06.2016
 * @author Anton Bondarev
 */

#ifndef AUDIO_DEV_H_
#define AUDIO_DEV_H_

#include <util/array.h>

struct audio_dev;

struct audio_dev_ops {
	void (*ad_ops_start)(struct audio_dev *dev);
	void (*ad_ops_pause)(struct audio_dev *dev);
	void (*ad_ops_resume)(struct audio_dev *dev);
	void (*ad_ops_stop)(struct audio_dev *dev);
};

struct audio_dev {
	struct audio_dev_ops *ad_ops;
	void *ad_priv;
};

#define AUDIO_DEV_DEF(ops, priv) \
	ARRAY_SPREAD_DECLARE(const struct audio_dev, __audio_device_registry); \
	ARRAY_SPREAD_ADD(__audio_device_registry, {ops, priv} )

#endif /* AUDIO_DEV_H_ */
