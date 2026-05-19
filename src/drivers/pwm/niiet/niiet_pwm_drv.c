/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>

#include <hal/system.h>

#include <kernel/time/time.h>

#include <drivers/pwm.h>
#include <drivers/pin_description.h>
#include <drivers/gpio.h>

#include "niiet_pwm.h"

extern int clk_enable(char *clk_name);

static inline uint32_t niiet_pwm_get_clk_div(struct niiet_pwm_priv *priv) {
    switch(priv->clk_div) {
        case 2:
            return TMR_CTRL_DIV(TMR_CTRL_DIV_2);
        case 4:
             return TMR_CTRL_DIV(TMR_CTRL_DIV_4);
        case 8:
             return TMR_CTRL_DIV(TMR_CTRL_DIV_8);
        case 1:
        default:
            return TMR_CTRL_DIV(TMR_CTRL_DIV_1);
    }
}

static inline int niiet_pwm_check_clk_div(struct niiet_pwm_priv *priv) {
    switch(priv->clk_div) {
        case 2:
            return 2;
        case 4:
             return 4;
        case 8:
             return 8;
        case 1:
        default:
            return 1;
    }
}

static inline void niiet_pwm_init_regs(struct pwm_device *dev) {
    struct niiet_tmr16_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;

    ctrl = 0;
    //ctrl |= TMR_CTRL_DIV(TMR_CTRL_DIV_8);
    ctrl |= niiet_pwm_get_clk_div(dev->pwmd_priv);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_STOP);
    regs->CTRL = ctrl;
}

static int niiet_pwm_init(struct pwm_device *dev) {
    struct niiet_pwm_priv *priv;

    int i;

    priv = dev->pwmd_priv;

    for (i = 0; i < NIIET_PWM_CHAN_MAX; i ++) {
        if (dev->pwmd_desc->pwmd_avail_chan_mask & (1 << i)) {
            const struct pin_description *pin;

            pin = &priv->pin_desc[i];
            gpio_setup_mode(pin->pd_port, (1 << pin->pd_pin), GPIO_MODE_ALT_SET(pin->pd_func));
        }
    }

    clk_enable((char *)priv->clk_name);
    niiet_pwm_init_regs(dev);

    dev->pwmd_base_freq = (SYS_CLOCK / niiet_pwm_check_clk_div(priv));
    dev->pwmd_max_period = (uint64_t)(priv->comp_mask * (NSEC_PER_SEC / dev->pwmd_base_freq)) ;

    return 0;
}

static int niiet_pwm_enable(struct pwm_device *dev, uint32_t chan_mask) {
    struct niiet_tmr16_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;

    ctrl = regs->CTRL;
    ctrl &= ~TMR_CTRL_MODE(TMR_CTRL_MODE_MASK);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_UP);
    regs->CTRL = ctrl;

    return 0;
}

static void niiet_pwm_disable(struct pwm_device *dev, uint32_t chan_mask) {
    struct niiet_tmr16_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;

    ctrl = regs->CTRL;
    ctrl &= ~TMR_CTRL_MODE(TMR_CTRL_MODE_MASK);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_STOP);
    regs->CTRL = ctrl;
}

static int niiet_pwm_set_period(struct pwm_device *dev, int period) {
    struct niiet_tmr16_regs *regs;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;
    regs->CAPCOM[0].CAPCOM_VAL = period - 1;

    return 0;
}

static int niiet_pwm_set_duty(struct pwm_device *dev, int chan_num, int duty) {
    struct niiet_tmr16_regs *regs;
    struct niiet_capcom_reg *capcom_reg;
    uint32_t cap_ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;
    capcom_reg = &regs->CAPCOM[chan_num];
    cap_ctrl = TMR_CAPCOM_CTRL_OUTMODE(TMR_CAPCOM_CTRL_OUTMODE_RESET_SET);
    capcom_reg->CAPCOM_CTRL = cap_ctrl;
    capcom_reg->CAPCOM_VAL = duty - 1;

    return 0;
}

struct pwm_ops niiet_pwm_ops = {
    .pwmo_init   = niiet_pwm_init,
    .pwmo_set_period = niiet_pwm_set_period,
    .pwmo_set_duty = niiet_pwm_set_duty,
    .pwmo_enable = niiet_pwm_enable,
    .pwmo_disable = niiet_pwm_disable
};