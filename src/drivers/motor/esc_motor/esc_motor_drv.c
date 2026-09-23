/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 31.08.2026
 */

#include <util/log.h>

#include <errno.h>
#include <string.h>

#include <drivers/motor.h>
#include <drivers/pwm.h>

#include <framework/mod/options.h>

/* DSHOT150 (in uS frame 106.72  bit 6.67 0bit 2.50	1bit 5.00) */
#define DSHOT150_FRAME  106720
#define DSHOT150_PERIOD   6670
#define DSHOT150_0BIT     2500
#define DSHOT150_1BIT     5000

/* DSHOT300 (in uS frame 53.28  bit 3.33 0bit 1.25	1bit 2.50) */
#define DSHOT300_FRAME   53280
#define DSHOT300_PERIOD   3330
#define DSHOT300_0BIT     1250
#define DSHOT300_1BIT     2500

/* DSHOT600 (in uS frame 26.72 bit 1.67 0bit 0.625	1bit 1.25) */
#define DSHOT600_FRAME   26720
#define DSHOT600_PERIOD   1670
#define DSHOT600_0BIT      625
#define DSHOT600_1BIT     1250

/* DSHOT1200 (in uS frame 13.28 bit 0.83 0bit 0.313	1bit 0.625) */
#define DSHOT1200_FRAME   13280
#define DSHOT1200_PERIOD    830
#define DSHOT1200_0BIT      313
#define DSHOT1200_1BIT      625

#define DSHOT_FRAME_SIZE      16
#define DSHOT_DMA_BUFFER_SIZE 18 /* resolution + frame reset (2us) */

static int esc_mo_init(struct motor_dev *dev) {
	dev->md_pwm_dev = pwm_dev_by_id(dev->md_pwm_id);
	if (dev->md_pwm_dev) {
		struct motor_conf conf;
		conf.mc_type = MOTOR_TYPE_DSHOT600;
		motor_conf(dev, &conf);
	}
	return 0;
}

static int esc_mo_conf(struct motor_dev *dev, struct motor_conf *conf) {
	if (!dev->md_pwm_dev) {
		return -EINVAL;
	}

	switch (conf->mc_type) {
	case MOTOR_TYPE_DSHOT150:
		dev->md_period = DSHOT150_PERIOD;
		break;
	case MOTOR_TYPE_DSHOT300:
		dev->md_period = DSHOT300_PERIOD;
		break;
	case MOTOR_TYPE_DSHOT600:
		dev->md_period = DSHOT600_PERIOD;
		break;
	case MOTOR_TYPE_DSHOT1200:
		dev->md_period = DSHOT1200_PERIOD;
		break;
	default:
		return -EINVAL;
	}

	pwm_set_period(dev->md_pwm_dev, dev->md_period);

	return 0;
}

static int esc_mo_send_msg(struct motor_dev *dev, struct motor_msg *msg) {
	uint16_t frame;
	int dma_buf[DSHOT_DMA_BUFFER_SIZE] = {0}; /* 16 + frame reset (2us)*/
	int bit0;
	int bit1;

	switch (dev->md_conf.mc_type) {
	case MOTOR_TYPE_DSHOT150:
		bit0 = DSHOT150_0BIT;
		bit1 = DSHOT150_1BIT;
		break;
	case MOTOR_TYPE_DSHOT300:
		bit0 = DSHOT300_0BIT;
		bit1 = DSHOT300_1BIT;
		break;
	case MOTOR_TYPE_DSHOT600:
		bit0 = DSHOT600_0BIT;
		bit1 = DSHOT600_1BIT;
		break;
	case MOTOR_TYPE_DSHOT1200:
		bit0 = DSHOT1200_0BIT;
		bit1 = DSHOT1200_1BIT;
		break;
	default:
		return -EINVAL;
	}

	frame = *((uint16_t *)msg->mm_buf);
	log_debug("esc_mo_send_msg(id(%d), frame(%0xx))", dev->md_id, frame);
	for (int i = 0; i < DSHOT_FRAME_SIZE; i++) {
		dma_buf[i] = (frame & 0x8000) ? bit1 : bit0;
		frame <<= 1;
	}

	pwm_set_duty_array(dev->md_pwm_dev, dev->md_pwm_chan, dma_buf,
	    DSHOT_DMA_BUFFER_SIZE);
	pwm_enable(dev->md_pwm_dev, 1 << dev->md_pwm_chan);
	return 0;
}

const struct motor_ops esc_motor_ops = {
    .mo_init = esc_mo_init,
	.mo_conf = esc_mo_conf,
    .mo_send_msg = esc_mo_send_msg,
};
