/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 15.05.2026
 */

#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <hal/system.h>

#include <kernel/time/time.h>

#include <drivers/pwm.h>
#include <drivers/pin_description.h>
#include <drivers/gpio.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include "stm32cube_pwm_priv.h"

extern int clk_enable(char *clk_name);

static int stm32cube_pwm_cube_init(struct stm32cube_pwm_priv *priv) {
    //CONF_PWM0_CLK_ENABLE_TIM();
    clk_enable((char *)priv->clk_name);

    memset(&priv->TimHandle, 0, sizeof(priv->TimHandle));  

    priv->TimHandle.Instance = (void *)priv->base_addr;
    priv->TimHandle.Init.Prescaler         = ((SYS_CLOCK / 2) / priv->tim_freq) - 1;
    priv->TimHandle.Init.ClockDivision     = 0;
    priv->TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    priv->TimHandle.Init.RepetitionCounter = 0;
    priv->TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    memset(&priv->sConfig, 0, sizeof(priv->sConfig));
     /* Common configuration for all channels */
    priv->sConfig.OCMode       = TIM_OCMODE_PWM1;
    priv->sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    priv->sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    priv->sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    priv->sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    priv->sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    return 0;
   
}


static int stm32cube_pwm_init(struct pwm_device *dev) {
    struct stm32cube_pwm_priv *priv;
    int i;

    priv = dev->pwmd_priv;

    stm32cube_pwm_cube_init(dev->pwmd_priv);

    for (i = 0; i < STM_PWM_CHAN_MAX; i ++) {
        if (dev->pwmd_desc->pwmd_avail_chan_mask & (1 << i)) {
            const struct pin_description *pin;

            pin = &priv->pin_desc[i];
            gpio_setup_mode(pin->pd_port, (1 << pin->pd_pin), 
                             GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP |
                             GPIO_MODE_ALT_SET(pin->pd_func));

        }
    }

    dev->pwmd_base_freq = priv->tim_freq;
    dev->pwmd_max_period = (uint64_t)(priv->comp_mask * (NSEC_PER_SEC / dev->pwmd_base_freq)) ;

    return 0;
}

static int stm32cube_pwm_enable(struct pwm_device *dev, uint32_t chan_mask) {
    return 0;
}

static void stm32cube_pwm_disable(struct pwm_device *dev, uint32_t chan_mask) {
}

static int stm32cube_pwm_set_period(struct pwm_device *dev, int period) {
    struct stm32cube_pwm_priv *priv = dev->pwmd_priv;

    priv->TimHandle.Init.Period            = period - 1;

    if (HAL_TIM_PWM_Init(&priv->TimHandle) != HAL_OK) {
        //log_error("Failed to init TIM PWM\n");
        return 0;
    }

    return 0;
}

#define STM32CUBE_PWM_WRONG_CHAN  (0xFFFFFFF)

static inline uint32_t stm32cube_pwm_chan_num_to_cube(int chan_num) {
    switch(chan_num) {
        case 1:
            return TIM_CHANNEL_1;
        case 2:
            return TIM_CHANNEL_2;
        case 3:
            return TIM_CHANNEL_3;
        case 4:
            return TIM_CHANNEL_3;
        default:
            return STM32CUBE_PWM_WRONG_CHAN;
    }
}

static int stm32cube_pwm_set_duty(struct pwm_device *dev, int chan_num, int duty) {
    struct stm32cube_pwm_priv *priv = dev->pwmd_priv;
    uint32_t chan;

    chan = stm32cube_pwm_chan_num_to_cube(chan_num);
    if (chan == STM32CUBE_PWM_WRONG_CHAN) {
        log_error("Wrong chan_num %d", chan_num);
        return -EINVAL;
    }
    
    /* Set the pulse value for channel */
    priv->sConfig.Pulse = duty;
    if (HAL_TIM_PWM_ConfigChannel(&priv->TimHandle, &priv->sConfig, chan) != HAL_OK)
    {
        log_error("Failed to config TIM PWM channel\n");
        return -EINVAL;
    }

    if (HAL_TIM_PWM_Start(&priv->TimHandle, chan) != HAL_OK) {
        //log_error("Failed to start TIM PWM\n");
        return 0;
    }

    return 0;
}

struct pwm_ops stm32cube_pwm_ops = {
    .pwmo_init   = stm32cube_pwm_init,
    .pwmo_set_period = stm32cube_pwm_set_period,
    .pwmo_set_duty = stm32cube_pwm_set_duty,
    .pwmo_enable = stm32cube_pwm_enable,
    .pwmo_disable = stm32cube_pwm_disable
};