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
#include <drivers/pin_description.h>

#include "niiet_pwm.h"

#define PWM_DEV_ID                1
#define PWM_DEV_PRIV_STRUCT_NAME  MACRO_CONCAT(pwm_dev_priv, PWM_DEV_ID)

#define CONF_PWM                  MACRO_CONCAT(CONF_PWM,PWM_DEV_ID)

#if defined CONF_PWM1_ENABLED

#define PWM_BASE_ADDR  MACRO_CONCAT(CONF_PWM,_REGION_BASE_ADDR)
#define PWM_CLK_NAME   MACRO_CONCAT(CONF_PWM,_CLK_ENABLE)
#define PWM_CHANNEL_NR MACRO_CONCAT(CONF_PWM,_CHANNEL)

#if defined(CONF_PWM1_PIN_OUT0_PORT)
#define PWM_CHAN0_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT0_PORT)
#define PWM_CHAN0_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT0_NR)
#define PWM_CHAN0_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT0_AF)
#define PWM_CHAN0_MASK_AVAIL (1 << 0)
#else
#define PWM_CHAN0_PORT       0
#define PWM_CHAN0_PIN        0
#define PWM_CHAN0_FUNC       (-1)
#define PWM_CHAN0_MASK_AVAIL (0 << 0)
#endif

#if defined(CONF_PWM1_PIN_OUT1_PORT)
#define PWM_CHAN1_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT1_PORT)
#define PWM_CHAN1_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT1_NR)
#define PWM_CHAN1_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT1_AF)
#define PWM_CHAN1_MASK_AVAIL (1 << 1)
#else
#define PWM_CHAN1_PORT       0
#define PWM_CHAN1_PIN        0
#define PWM_CHAN1_FUNC       (-1)
#define PWM_CHAN1_MASK_AVAIL (0 << 1)
#endif

#if defined(CONF_PWM1_PIN_OUT2_PORT)
#define PWM_CHAN2_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT2_PORT)
#define PWM_CHAN2_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT2_NR)
#define PWM_CHAN2_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT2_AF)
#define PWM_CHAN2_MASK_AVAIL (1 << 2)
#else
#define PWM_CHAN2_PORT       0
#define PWM_CHAN2_PIN        0
#define PWM_CHAN2_FUNC       (-1)
#define PWM_CHAN2_MASK_AVAIL (0 << 2)
#endif

#if defined(CONF_PWM1_PIN_OUT3_PORT)
#define PWM_CHAN3_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT3_PORT)
#define PWM_CHAN3_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT3_NR)
#define PWM_CHAN3_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT3_AF)
#define PWM_CHAN3_MASK_AVAIL (1 << 3)
#else
#define PWM_CHAN3_PORT       0
#define PWM_CHAN3_PIN        0
#define PWM_CHAN3_FUNC       (-1)
#define PWM_CHAN3_MASK_AVAIL (1 << 3)
#endif

#endif

#define PWM_AVAIL_MASK   \
            ( \
                PWM_CHAN0_MASK_AVAIL | \
                PWM_CHAN1_MASK_AVAIL | \
                PWM_CHAN2_MASK_AVAIL | \
                PWM_CHAN3_MASK_AVAIL \
            )

extern struct pwm_ops niiet_pwm_ops;

static const struct pin_description pwm_pin_desc[NIIET_PWM_CHAN_MAX] = {
    {
        .pd_port = PWM_CHAN0_PORT,
        .pd_pin = PWM_CHAN0_PIN,
        .pd_func = PWM_CHAN0_FUNC,
    },
    {
        .pd_port = PWM_CHAN1_PORT,
        .pd_pin = PWM_CHAN1_PIN,
        .pd_func = PWM_CHAN1_FUNC,
    },
    {
        .pd_port = PWM_CHAN2_PORT,
        .pd_pin = PWM_CHAN2_PIN,
        .pd_func = PWM_CHAN2_FUNC,
    },
    {
        .pd_port = PWM_CHAN3_PORT,
        .pd_pin = PWM_CHAN3_PIN,
        .pd_func = PWM_CHAN3_FUNC,
    },
};

static struct niiet_pwm_priv PWM_DEV_PRIV_STRUCT_NAME = {
    .pin_desc  = &pwm_pin_desc[0],
    .base_addr = PWM_BASE_ADDR,
    .clk_name  = PWM_CLK_NAME(),
    .channel   = PWM_CHANNEL_NR(),
};

PWM_DEV_DEF(PWM_DEV_ID, &niiet_pwm_ops, &PWM_DEV_PRIV_STRUCT_NAME,
                        &pwm_pin_desc[0], PWM_BASE_ADDR, PWM_AVAIL_MASK);