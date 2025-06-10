/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>

#include <drivers/pwm.h>

#include <drivers/gpio.h>

#include "niiet_pwm.h"

extern int clk_enable(char *clk_name);

int niiet_pwm_init(struct pwm_device *dev) {
    struct niiet_pwm_priv *priv;
    struct pin_description *pin;

    priv = dev->pwmd_priv;
    dev->pwmd_base_addr = priv->base_addr;
    dev->pwmd_pin = priv->pin_desc; /* copy */
    pin = &priv->pin_desc;

    gpio_setup_mode(pin->pd_port, (1 << pin->pd_pin), pin->pd_func);

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