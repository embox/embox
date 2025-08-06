/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>
#include <errno.h>
#include <assert.h>

#include <drivers/pwm.h>
#include <drivers/pin_description.h>
#include <drivers/gpio.h>

#include "niiet_pwm.h"

extern int clk_enable(char *clk_name);

static inline void niiet_pwm_init_regs(struct pwm_device *dev) {
    struct niiet_tmr16_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_base_addr;

    ctrl = 0;
    ctrl |= TMR_CTRL_DIV(TMR_CTRL_DIV_8);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_STOP);
    regs->CTRL = ctrl;
}

static int niiet_pwm_init(struct pwm_device *dev) {
    struct niiet_pwm_priv *priv;
    const struct pin_description *pin;

    priv = dev->pwmd_priv;
    pin = priv->pin_desc;

    gpio_setup_mode(pin->pd_port, (1 << pin->pd_pin), GPIO_MODE_ALT_SET(pin->pd_func));
    clk_enable((char *)priv->clk_name);
    niiet_pwm_init_regs(dev);

    return 0;
}

static int niiet_pwm_enable(struct pwm_device *dev) {
    struct niiet_tmr16_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_base_addr;

    ctrl = regs->CTRL;
    ctrl &= ~TMR_CTRL_MODE(TMR_CTRL_MODE_MASK);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_UP);
    regs->CTRL = ctrl;

    return 0;
}

static void niiet_pwm_disable(struct pwm_device *dev) {
    struct niiet_tmr16_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_base_addr;

    ctrl = regs->CTRL;
    ctrl &= ~TMR_CTRL_MODE(TMR_CTRL_MODE_MASK);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_STOP);
    regs->CTRL = ctrl;
}

int niiet_pwm_config(struct pwm_device *dev, int duty, int period) {
    struct niiet_tmr16_regs *regs;
    struct niiet_pwm_priv   *priv;
    struct niiet_capcom_reg *capcom_reg;
    uint32_t cap_ctrl;

    assert(dev);

    priv = dev->pwmd_priv;
    if (priv->channel > 3) {
        return -EINVAL;
    }

    regs = (void *)dev->pwmd_base_addr;
    capcom_reg = &regs->CAPCOM[priv->channel];
    cap_ctrl = TMR_CAPCOM_CTRL_OUTMODE(TMR_CAPCOM_CTRL_OUTMODE_RESET_SET);
    regs->CAPCOM[0].CAPCOM_VAL = period - 1;
    capcom_reg->CAPCOM_CTRL = cap_ctrl;
    capcom_reg->CAPCOM_VAL = duty - 1;

    return 0;
}

struct pwm_ops niiet_pwm_ops = {
    .pwmo_init   = niiet_pwm_init,
    .pwmo_config = niiet_pwm_config,
    .pwmo_enable = niiet_pwm_enable,
    .pwmo_disable = niiet_pwm_disable
};