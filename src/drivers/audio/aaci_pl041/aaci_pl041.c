/**
 * @file
 *
 * @date 27.09.2016
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdint.h>

#include <util/log.h>

#include <drivers/audio/portaudio.h>
#include <drivers/audio/audio_dev.h>

#include "aaci_pl041.h"

#define AACI_MAXBUF_LEN_MAX_BUF_LEN 0x10000


struct aaci_pl041_hw_dev {
	uint32_t base_addr;
};

static struct aaci_pl041_hw_dev aaci_pl041_hw_dev;


struct aaci_pl041_dev_priv {
	struct aaci_pl041_hw_dev *hw_dev;
	int devid;
	uint8_t *out_buf;
	uint32_t out_buf_len;
	uint8_t *in_buf;
	uint32_t in_buf_len;

	uint32_t cur_buff_offset;
};

static void aaci_pl041_dev_start(struct audio_dev *dev) {
}

static void aaci_pl041_dev_pause(struct audio_dev *dev) {
}

static void aaci_pl041_dev_resume(struct audio_dev *dev) {
}

static void aaci_pl041_dev_stop(struct audio_dev *dev) {
}

static int aaci_pl041_ioctl(struct audio_dev *dev, int cmd, void *args) {
	SET_ERRNO(EINVAL);
	return -1;
}

static const struct audio_dev_ops aaci_pl041_dev_ops = {
	.ad_ops_start  = aaci_pl041_dev_start,
	.ad_ops_pause  = aaci_pl041_dev_pause,
	.ad_ops_resume = aaci_pl041_dev_resume,
	.ad_ops_stop   = aaci_pl041_dev_stop,
	.ad_ops_ioctl  = aaci_pl041_ioctl
};

static uint8_t dac1_out_buf[AACI_MAXBUF_LEN_MAX_BUF_LEN] __attribute__ ((aligned(0x1000)));
static uint8_t adc1_in_buf[AACI_MAXBUF_LEN_MAX_BUF_LEN] __attribute__ ((aligned(0x1000)));

static struct aaci_pl041_dev_priv aaci_pl041_dac1 = {
	.hw_dev      = &aaci_pl041_hw_dev,
	.devid       = 0,
	.out_buf     = dac1_out_buf,
	.out_buf_len = sizeof(dac1_out_buf),
};

static struct aaci_pl041_dev_priv aaci_pl041_dac2 = {
	.hw_dev = &aaci_pl041_hw_dev,
	.devid  = 1
};

static struct aaci_pl041_dev_priv aaci_pl041_adc1 = {
	.hw_dev      = &aaci_pl041_hw_dev,
	.devid       = 2,
	.in_buf      = adc1_in_buf,
	.in_buf_len  = sizeof(adc1_in_buf)
};

AUDIO_DEV_DEF("aaci_pl041_dac1", (struct audio_dev_ops *)&aaci_pl041_dev_ops, &aaci_pl041_dac1);
AUDIO_DEV_DEF("aaci_pl041_dac2", (struct audio_dev_ops *)&aaci_pl041_dev_ops, &aaci_pl041_dac2);
AUDIO_DEV_DEF("aaci_pl041_adc1", (struct audio_dev_ops *)&aaci_pl041_dev_ops, &aaci_pl041_adc1);

uint8_t *audio_dev_get_out_cur_ptr(struct audio_dev *audio_dev) {
	return NULL;
}
