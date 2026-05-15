/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 15.05.2026
 */

#include <util/log.h>

#include <stddef.h>

#include <assert.h>

#include <drivers/pwm.h>


static int skeleton_pwm_init(struct pwm_device *dev) {
    return 0;
}

static int skeleton_pwm_enable(struct pwm_device *dev, uint32_t chan_mask) {
    return 0;
}

static void skeleton_pwm_disable(struct pwm_device *dev, uint32_t chan_mask) {
}

static int skeleton_pwm_set_period(struct pwm_device *dev, int period) {

    return 0;
}

static int skeleton_pwm_set_duty(struct pwm_device *dev, int chan_num, int duty) {

    return 0;
}

struct pwm_ops skeleton_pwm_ops = {
    .pwmo_init   = skeleton_pwm_init,
    .pwmo_set_period = skeleton_pwm_set_period,
    .pwmo_set_duty = skeleton_pwm_set_duty,
    .pwmo_enable = skeleton_pwm_enable,
    .pwmo_disable = skeleton_pwm_disable
};