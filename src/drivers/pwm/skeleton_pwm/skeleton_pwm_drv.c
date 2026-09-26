/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 15.05.2026
 */

#include <assert.h>
#include <stddef.h>

#include <drivers/pwm.h>
#include <util/log.h>

#include "skeleton_pwm_priv.h"

static int skeleton_pwm_init(struct pwm_device *dev) {
	struct skeleton_pwm_priv *priv;

	priv = dev->pwmd_priv;

	pwm_dma_config(dev, 0, priv->dma_buffer[0], SKELETON_PWM_DMA_BUF_SIZE);

    dev->pwmd_base_freq = priv->freq;
    if (priv->freq < 1000000000) {
        dev->pwmd_max_period = 1000000000 / dev->pwmd_base_freq;
    } else {
        dev->pwmd_max_period = (dev->pwmd_base_freq / 1000000000) * 1000000000;
    }

	return 0;
}

static int skeleton_pwm_enable(struct pwm_device *dev, uint32_t chan_mask) {
	log_debug("skeleton_pwm_enable(id(%d) mask(0x%x)", dev->pwmd_id, chan_mask);
	return 0;
}

static void skeleton_pwm_disable(struct pwm_device *dev, uint32_t chan_mask) {
	log_debug("skeleton_pwm_disable(id(%d) mask(0x%x)", dev->pwmd_id, chan_mask);
}

static int skeleton_pwm_set_period(struct pwm_device *dev, int period) {
	log_debug("skeleton_pwm_set_period(id(%d) period(0x%x)", dev->pwmd_id, period);
	return 0;
}

static int skeleton_pwm_set_duty(struct pwm_device *dev, int chan_num, int duty) {
	return 0;
}

static int skeleton_pwm_set_duty_array(struct pwm_device *dev, int chan_num,
    int duty_ns[], int size) {
	log_debug("skeleton_pwm_set_duty_array(id(%d) chan(0x%x) size(%d)",
	    dev->pwmd_id, chan_num, size);
	for (int i = 0; i < size; i += 4) {
		log_debug(" %3d: %10d  %10d %10d %10d", i, duty_ns[i + 0],
		    duty_ns[i + 1], duty_ns[i + 2], duty_ns[i + 3]);
	}

	return 0;
}

struct pwm_ops skeleton_pwm_ops = {
	.pwmo_init = skeleton_pwm_init,
    .pwmo_set_period = skeleton_pwm_set_period,
    .pwmo_set_duty = skeleton_pwm_set_duty,
    .pwmo_set_duty_array = skeleton_pwm_set_duty_array,
    .pwmo_enable = skeleton_pwm_enable,
    .pwmo_disable = skeleton_pwm_disable
};