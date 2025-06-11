/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>

#include <util/macro.h>

#include <config/board_config.h>
#include <drivers/gpio.h>

#include <drivers/pwm.h>

#include "niiet_pwm.h"

#define PWM_DEV_ID                0
#define PWM_DEV_PRIV_STRUCT_NAME  MACRO_CONCAT(pwm_dev_priv, PWM_DEV_ID)

#define CONF_PWM       MACRO_CONCAT(CONF_PWM,PWM_DEV_ID)

#if defined CONF_PWM0_ENABLED
#define PWM_BASE_ADDR  MACRO_CONCAT(CONF_PWM,_REGION_BASE_ADDR)
#define PWM_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT_PORT)
#define PWM_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT_NR)
#define PWM_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT_AF)
#define PWM_CLK_NAME   MACRO_CONCAT(CONF_PWM,_CLK_ENABLE)
#define PWM_CHANNEL_NR MACRO_CONCAT(CONF_PWM,_CHANNEL)
#endif

extern struct pwm_ops niiet_pwm_ops;

static const struct pin_description pwm_pin_desc = {
        .pd_port = PWM_PORT,
        .pd_pin = PWM_PIN,
        .pd_func = PWM_FUNC
};

static struct niiet_pwm_priv PWM_DEV_PRIV_STRUCT_NAME = {
    .pin_desc  = &pwm_pin_desc,
    .base_addr = PWM_BASE_ADDR,
    .clk_name  = PWM_CLK_NAME(),
    .channel   = PWM_CHANNEL_NR(),
};

PWM_DEV_DEF(PWM_DEV_ID, &niiet_pwm_ops, &PWM_DEV_PRIV_STRUCT_NAME,
                        &pwm_pin_desc, PWM_BASE_ADDR);