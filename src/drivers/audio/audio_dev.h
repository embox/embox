/**
 * @file
 *
 * @date 13.06.2016
 * @author Anton Bondarev
 */

#ifndef AUDIO_DEV_H_
#define AUDIO_DEV_H_

#include <stdint.h>

#include <lib/libds/array.h>

struct audio_dev;

struct audio_dev_ops {
	void (*ad_ops_start)(struct audio_dev *dev);
	void (*ad_ops_pause)(struct audio_dev *dev);
	void (*ad_ops_resume)(struct audio_dev *dev);
	void (*ad_ops_stop)(struct audio_dev *dev);
	void (*ad_ops_add_sample)(struct audio_dev *dev, uint8_t *buf);
	int (*ad_ops_ioctl)(struct audio_dev *dev, int cmd, void *args);
};

typedef enum {
	AUDIO_DEV_OUTPUT,
	AUDIO_DEV_INPUT,
	AUDIO_DEV_BOTH
} audio_dev_dir_t;

struct audio_dev {
	struct audio_dev_ops *ad_ops;
	const char *ad_name;
	void *ad_priv;
	audio_dev_dir_t dir;
};

#define AUDIO_DEV_DEF(name, ops, priv, dir) \
	ARRAY_SPREAD_DECLARE(const struct audio_dev, __audio_device_registry); \
	ARRAY_SPREAD_ADD(__audio_device_registry, {ops,name, priv, dir} )

extern struct audio_dev *audio_dev_get_by_idx(int idx);

extern struct audio_dev *audio_dev_get_by_name(char name[]);

extern uint8_t *audio_dev_get_in_cur_ptr(struct audio_dev *audio_dev);
extern uint8_t *audio_dev_get_out_cur_ptr(struct audio_dev *audio_dev);

extern void audio_dev_open_out_stream(struct audio_dev *audio_dev,
	void *stream);
extern void audio_dev_open_in_stream(struct audio_dev *audio_dev,
	void *stream);

/* ioctl commands */
#define ADIOCTL_IN_SUPPORT  1
#define ADIOCTL_OUT_SUPPORT 2
#define ADIOCTL_BUFLEN      3
#define ADIOCTL_SET_RATE    6
#define ADIOCTL_GET_RATE    7

/* ioctl support list */
#define AD_MONO_SUPPORT    (1 << 0)
#define AD_STEREO_SUPPORT  (1 << 1)
#define AD_8BIT_SUPPORT    (1 << 3)
#define AD_16BIT_SUPPORT   (1 << 4)

#endif /* AUDIO_DEV_H_ */
