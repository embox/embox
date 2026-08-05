/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 15.05.2026
 */

#include <stddef.h>

#include <util/macro.h>

#include <drivers/pwm.h>
#include <drivers/pin_description.h>

#include <framework/mod/options.h>

#include "skeleton_pwm_priv.h"

#define PWM_DEV_ID                1

#define PWM_DEV_PRIV_STRUCT_NAME  MACRO_CONCAT(pwm_dev_priv, PWM_DEV_ID)

#define CONF_PWM                  MACRO_CONCAT(CONF_PWM,PWM_DEV_ID)

#define USE_BCONF        OPTION_GET(BOOLEAN, use_bconf)
#define USE_DMA          OPTION_GET(BOOLEAN, use_dma)

#if USE_BCONF
#include <config/board_config.h>
#include <drivers/gpio.h>

#define PWM_BASE_ADDR  MACRO_CONCAT(CONF_PWM,_REGION_BASE_ADDR)
#define PWM_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT_PORT)
#define PWM_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT_NR)
#define PWM_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT_AF)
#define PWM_CLK_NAME   MACRO_CONCAT(CONF_PWM,_CLK_ENABLE)
#define PWM_CHANNEL_NR MACRO_CONCAT(CONF_PWM,_CHANNEL)

static const struct pin_description pwm_pin_desc = {
        .pd_port = PWM_PORT,
        .pd_pin = PWM_PIN,
        .pd_func = PWM_FUNC
};

#define PTR_PIN_DESC  (&pwm_pin_desc)

#else
#define PWM_BASE_ADDR     0
#define PWM_PORT          0
#define PWM_PIN           0
#define PWM_FUNC          0
#define PWM_CLK_NAME()    MACRO_STRING(MACRO_CONCAT(MACRO_CONCAT(PWM,PWM_DEV_ID),_CLK))
#define PWM_CHANNEL_NR()  0
#define PTR_PIN_DESC      (NULL)

#if USE_DMA
static uint32_t skeleton_dmas[SKELETON_PWM_CHAN_MAX] = {
    1
};
#define PWM_DMAS    (&skeleton_dmas[0])

#else
#define PWM_DMAS    (NULL)
#endif /* USE_DMA */

#endif /* USE_BCONF*/

#define CHAN_AVAIL_MASK \
         (1 << 0 )

extern struct pwm_ops skeleton_pwm_ops;

static struct skeleton_pwm_priv PWM_DEV_PRIV_STRUCT_NAME = {
    .pin_desc  = PTR_PIN_DESC,
    .base_addr = PWM_BASE_ADDR,
    .clk_name  = PWM_CLK_NAME(),
    .idx       = PWM_DEV_ID,
};

PWM_DEV_DEF(PWM_DEV_ID, &skeleton_pwm_ops, &PWM_DEV_PRIV_STRUCT_NAME,
                        PTR_PIN_DESC, PWM_BASE_ADDR,
                        1 << 0 /* 0 chan avail */,
                        SKELETON_PWM_CHAN_MAX /* max chan */,
                        PWM_DMAS);