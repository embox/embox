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
	const char *ad_name;
	void *ad_priv;
};

#define AUDIO_DEV_DEF(name, ops, priv) \
	ARRAY_SPREAD_DECLARE(const struct audio_dev, __audio_device_registry); \
	ARRAY_SPREAD_ADD(__audio_device_registry, {ops,name, priv} )

extern struct audio_dev *audio_dev_get_by_idx(int idx);

extern struct audio_dev *audio_dev_get_by_name(char name[]);

#endif /* AUDIO_DEV_H_ */
