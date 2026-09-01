/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 31.08.2026
 */

#include <util/log.h>

#include <drivers/motor.h>
#include <drivers/pwm.h>

#include <framework/mod/options.h>

/* DSHOT600 (frame 26.72uS bit 1.67 0bit 0.625	1bit 1.25) */
#define DSHOT600_FRAME  26720
#define DSHOT600_PERIOD 1670
#define DSHOT600_0BIT   625
#define DSHOT600_1BIT   1250

#define DSHOT_FRAME_SIZE      16
#define DSHOT_DMA_BUFFER_SIZE 18 /* resolution + frame reset (2us) */

static int esc_mo_init(struct motor_dev *dev) {
	dev->md_pwm_dev = pwm_dev_by_id(dev->md_pwm_id);
	if (dev->md_pwm_dev) {
		pwm_set_period(dev->md_pwm_dev, DSHOT600_PERIOD);
		dev->md_period = DSHOT600_PERIOD;
	}
	return 0;
}

static int esc_mo_send_msg(struct motor_dev *dev, struct motor_msg *msg) {
	uint16_t frame;
	int dma_buf[DSHOT_DMA_BUFFER_SIZE] = {0}; /* 16 + frame reset (2us)*/

	frame = *((uint16_t *)msg->mm_buf);
	log_debug("esc_mo_send_msg(id(%d), frame(%0xx))", dev->md_id, frame);
	for (int i = 0; i < DSHOT_FRAME_SIZE; i++) {
		dma_buf[i] = (frame & 0x8000) ? DSHOT600_1BIT : DSHOT600_0BIT;
		frame <<= 1;
	}

	pwm_set_duty_array(dev->md_pwm_dev, dev->md_pwm_chan, dma_buf,
	    DSHOT_DMA_BUFFER_SIZE);

	return 0;
}

const struct motor_ops esc_motor_ops = {
    .mo_init = esc_mo_init,
    .mo_send_msg = esc_mo_send_msg,
};
