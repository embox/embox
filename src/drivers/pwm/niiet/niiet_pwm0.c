/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>

#include <util/macro.h>

#include <drivers/pwm.h>

#include "niiet_pwm.h"

#define PWM_DEV_ID                0
#define PWM_DEV_PRIV_STRUCT_NAME  MACRO_CONCAT(pwm_dev_priv, PWM_DEV_ID)

extern struct pwm_ops niiet_pwm_ops;

struct niiet_pwm_priv    PWM_DEV_PRIV_STRUCT_NAME = {
    .pin_desc = {
        .pd_port = 0,
        .pd_pin = 0,
        .pd_func = 0
    },
    .base_addr = 0,
};

PWM_DEV_DEF(PWM_DEV_ID, &niiet_pwm_ops, &PWM_DEV_PRIV_STRUCT_NAME);