/**
 * @file
 *
 * @date 13.06.2016
 * @author Anton Bondarev
 */

#ifndef AUDIO_DEV_H_
#define AUDIO_DEV_H_

#include <stdint.h>

#include <util/array.h>

struct audio_dev;

struct audio_dev_ops {
	void (*ad_ops_start)(struct audio_dev *dev);
	void (*ad_ops_pause)(struct audio_dev *dev);
	void (*ad_ops_resume)(struct audio_dev *dev);
	void (*ad_ops_stop)(struct audio_dev *dev);
	void (*ad_ops_add_sample)(struct audio_dev *dev, uint8_t *buf);
	int (*ad_ops_ioctl)(struct audio_dev *dev, int cmd, void *args);
};

struct audio_dev {
	struct audio_dev_ops *ad_ops;
	const char *ad_name;
	void *ad_priv;
	size_t buf_len;
	uint8_t num_of_chan;
	uint8_t max_chan;
};

#define AUDIO_DEV_DEF(name, ops, priv) \
	ARRAY_SPREAD_DECLARE(const struct audio_dev, __audio_device_registry); \
	ARRAY_SPREAD_ADD(__audio_device_registry, {ops,name, priv} )

extern struct audio_dev *audio_dev_get_by_idx(int idx);

extern struct audio_dev *audio_dev_get_by_name(char name[]);

extern uint8_t *audio_dev_get_out_cur_ptr(struct audio_dev *audio_dev);

/* ioctl commands */
#define ADIOCTL_SUPPORT 2
#define ADIOCTL_BUFLEN  3

/* ioctl support list */
#define AD_MONO_SUPPORT    (1 << 0)
#define AD_STEREO_SUPPORT   (1 << 1)
#define AD_8BIT_SUPPORT    (1 << 3)
#define AD_16BIT_SUPPORT   (1 << 4)

#endif /* AUDIO_DEV_H_ */
