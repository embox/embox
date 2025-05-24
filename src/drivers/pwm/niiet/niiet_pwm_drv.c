/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>

#include <drivers/pwm.h>

int niiet_pwm_init(struct pwm_device *dev) {
    return 0;
}

int niiet_pwm_config(struct pwm_device *dev,  int duty_ns, int period_ns) {
    return 0;
}

int niiet_pwm_enable(struct pwm_device *dev) {
    return 0;
}

void niiet_pwm__disable(struct pwm_device *dev) {

}

struct pwm_ops niiet_pwm_ops = {
    .pwmo_init   = niiet_pwm_init,
    .pwmo_config = niiet_pwm_config,
    .pwmo_enable = niiet_pwm_enable,
    .pwmo_disable = niiet_pwm__disable
};