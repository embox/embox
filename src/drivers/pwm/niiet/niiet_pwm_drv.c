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

#include "niiet_pwm_priv.h"
#include "niiet_pwm_regs.h"

extern int clk_enable(char *clk_name);

#if VG1T_VERSION == 0 /*vg015*/
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

static inline void niiet_pwm_set_clk_div(struct niiet_tmr_regs *regs, int cd) {
    uint32_t ctrl;

    ctrl = regs->CTRL;
    ctrl &= ~TMR_CTRL_DIV(TMR_CTRL_MODE_MASK);
    ctrl |= TMR_CTRL_DIV(cd);
    regs->CTRL = ctrl;
}

static inline void niiet_pwm_tmr_en(struct niiet_pwm_priv *priv) {
    clk_enable((char *)priv->clk_name);
}

static inline void niiet_pwm_set_period_reg(struct niiet_tmr_regs *regs, int p) {
    regs->CAPCOM[0].CAPCOM_VAL = p - 1;
}

static inline void niiet_pwm_outen(struct niiet_tmr_regs *regs, int chan) {
}

#else /* VG1T_VERSION != 0 */

static inline uint32_t niiet_pwm_get_clk_div(struct niiet_pwm_priv *priv) {
    return priv->clk_div;
}

static inline int niiet_pwm_check_clk_div(struct niiet_pwm_priv *priv) {
    return priv->clk_div;
}

static inline void niiet_pwm_set_clk_div(struct niiet_tmr_regs *regs, int cd) {
    regs->CLKDIV = cd - 1;
}

static inline void niiet_pwm_tmr_en(struct niiet_pwm_priv *priv) {
    extern int sys_ctrl_enable_tmr(int num) ;
    extern int sys_ctrl_enable_dev(const char *name);

    clk_enable((char *)priv->clk_name);

    sys_ctrl_enable_tmr(1);
}

static inline void niiet_pwm_set_period_reg(struct niiet_tmr_regs *regs, int p) {
    regs->PERIOD = p - 1;
}

static inline void niiet_pwm_outen(struct niiet_tmr_regs *regs, int chan) {
    regs->CTRL |= TMR_CTRL_OUTEN(chan);
}

#endif /* VG1T_VERSION == 0*/

static inline void niiet_pwm_init_regs(struct pwm_device *dev) {
    struct niiet_tmr_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;

    ctrl = 0;
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_STOP);
    regs->CTRL = ctrl;

    niiet_pwm_set_clk_div(regs, niiet_pwm_get_clk_div(dev->pwmd_priv) );
}

static int niiet_pwm_init(struct pwm_device *dev) {
    struct niiet_pwm_priv *priv;

    int i;

    priv = dev->pwmd_priv;

    niiet_pwm_tmr_en(priv);

    niiet_pwm_init_regs(dev);

    for (i = 0; i < NIIET_PWM_CHAN_MAX; i ++) {
        if (dev->pwmd_desc->pwmd_avail_chan_mask & (1 << i)) {
            const struct pin_description *pin;

            pin = &priv->pin_desc[i];
            gpio_setup_mode(pin->pd_port, (1 << pin->pd_pin), GPIO_MODE_ALT_SET(pin->pd_func));

            niiet_pwm_outen((void *)dev->pwmd_desc->pwmd_base_addr, i);
        }
    }

    dev->pwmd_base_freq = (SYS_CLOCK / niiet_pwm_check_clk_div(priv));
    dev->pwmd_max_period = (uint64_t)(priv->comp_mask * (NSEC_PER_SEC / dev->pwmd_base_freq)) ;

    return 0;
}

static int niiet_pwm_enable(struct pwm_device *dev, uint32_t chan_mask) {
    struct niiet_tmr_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;

    //capcom_reg = &regs->CAPCOM[chan_num];
    //cap_ctrl = TMR_CAPCOM_CTRL_OUTMODE(TMR_CAPCOM_CTRL_OUTMODE_RESET_SET);

    ctrl = regs->CTRL;
    ctrl &= ~TMR_CTRL_MODE(TMR_CTRL_MODE_MASK);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_UP);
    regs->CTRL = ctrl;

    return 0;
}

static void niiet_pwm_disable(struct pwm_device *dev, uint32_t chan_mask) {
    struct niiet_tmr_regs *regs;
    uint32_t ctrl;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;

    //capcom_reg = &regs->CAPCOM[chan_num];
    //cap_ctrl = TMR_CAPCOM_CTRL_OUTMODE(TMR_CAPCOM_CTRL_OUTMODE_RESET_SET);

    ctrl = regs->CTRL;
    ctrl &= ~TMR_CTRL_MODE(TMR_CTRL_MODE_MASK);
    ctrl |= TMR_CTRL_MODE(TMR_CTRL_MODE_STOP);
    regs->CTRL = ctrl;
}

static int niiet_pwm_set_period(struct pwm_device *dev, int period) {
    struct niiet_tmr_regs *regs;

    assert(dev);

    regs = (void *)dev->pwmd_desc->pwmd_base_addr;

    niiet_pwm_set_period_reg(regs, period);

    return 0;
}

static int niiet_pwm_set_duty(struct pwm_device *dev, int chan_num, int duty) {
    struct niiet_tmr_regs *regs;
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