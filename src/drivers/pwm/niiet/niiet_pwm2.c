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

#include <framework/mod/options.h>

#include "niiet_pwm_priv.h"

#define PWM_DEV_ID                2
#define PWM_DEV_PRIV_STRUCT_NAME  MACRO_CONCAT(pwm_dev_priv, PWM_DEV_ID)

#define CONF_PWM                  MACRO_CONCAT(CONF_PWM,PWM_DEV_ID)

#if defined CONF_PWM2_ENABLED

#define PWM_BASE_ADDR        MACRO_CONCAT(CONF_PWM,_REGION_BASE_ADDR)
#define PWM_CLK_NAME         MACRO_CONCAT(CONF_PWM,_CLK_ENABLE)
#define PWM_CHANNEL_NR       MACRO_CONCAT(CONF_PWM,_CHANNEL)
#define PWM_COMP_MASK        MACRO_CONCAT(CONF_PWM,_MISC_COMP_MASK)

#if defined(CONF_PWM2_PIN_OUT0_PORT)
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

#if defined(CONF_PWM2_PIN_OUT1_PORT)
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

#if defined(CONF_PWM2_PIN_OUT2_PORT)
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

#if defined(CONF_PWM2_PIN_OUT3_PORT)
#define PWM_CHAN3_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT3_PORT)
#define PWM_CHAN3_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT3_NR)
#define PWM_CHAN3_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT3_AF)
#define PWM_CHAN3_MASK_AVAIL (1 << 3)
#else
#define PWM_CHAN3_PORT       0
#define PWM_CHAN3_PIN        0
#define PWM_CHAN3_FUNC       (-1)
#define PWM_CHAN3_MASK_AVAIL (0 << 3)
#endif

#if defined(CONF_PWM2_DMA_OUT0_NUM)
#define DMA0_EN     1
#define DMA0_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT0_NUM)
#define DMA0_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT0_CHAN)
#define DMA0_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT0)
#else
#define DMA0_EN     0
#define DMA0_NUM    0
#define DMA0_CHAN   0
#endif

#if defined(CONF_PWM2_DMA_OUT1_NUM)
#define DMA1_EN     1
#define DMA1_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT1_NUM)
#define DMA1_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT1_CHAN)
#define DMA1_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT1)
#else
#define DMA1_EN     0
#define DMA1_NUM    0
#define DMA1_CHAN   0
#endif

#if defined(CONF_PWM2_DMA_OUT2_NUM)
#define DMA2_EN     1
#define DMA2_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT2_NUM)
#define DMA2_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT2_CHAN)
#define DMA2_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT2)
#else
#define DMA2_EN     0
#define DMA2_NUM    0
#define DMA2_CHAN   0
#endif

#if defined(CONF_PWM2_DMA_OUT3_NUM)
#define DMA3_EN     1
#define DMA3_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT3_NUM)
#define DMA3_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT3_CHAN)
#define DMA3_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT3)
#else
#define DMA3_EN     0
#define DMA3_NUM    0
#define DMA3_CHAN   0
#endif

#endif /* defined CONF_PWM2_ENABLED */

#if DMA0_EN || DMA1_EN || DMA2_EN || DMA3_EN

static uint32_t pwm_dmas[NIIET_PWM_CHAN_MAX] = {
    (DMA0_EN << NIIET_PWM_DMA_EN_OFF) |
        (DMA0_NUM << NIIET_PWM_DMA_NUM_OFF) |
        (DMA0_CHAN << NIIET_PWM_DMA_CHAN_OFF),
    (DMA1_EN << NIIET_PWM_DMA_EN_OFF) |
        (DMA1_NUM << NIIET_PWM_DMA_NUM_OFF) |
        (DMA1_CHAN << NIIET_PWM_DMA_CHAN_OFF)),
    (DMA2_EN << NIIET_PWM_DMA_EN_OFF) |
        (DMA2_NUM << NIIET_PWM_DMA_NUM_OFF) |
        (DMA2_CHAN << NIIET_PWM_DMA_CHAN_OFF),
    (DMA3_EN << NIIET_PWM_DMA_EN_OFF) |
        (DMA3_NUM << NIIET_PWM_DMA_NUM_OFF) |
        (DMA3_CHAN << NIIET_PWM_DMA_CHAN_OFF),
};
#define PWM_DMAS (&pwm_dmas[0])

#else

#define PWM_DMAS (NULL)

#endif /* DMA0_EN || DMA1_EN || DMA2_EN || DMA3_EN */

#define PWM_AVAIL_MASK   \
            ( \
                PWM_CHAN0_MASK_AVAIL | \
                PWM_CHAN1_MASK_AVAIL | \
                PWM_CHAN2_MASK_AVAIL | \
                PWM_CHAN3_MASK_AVAIL \
            )

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

extern struct pwm_ops niiet_pwm_ops;

static struct niiet_pwm_priv PWM_DEV_PRIV_STRUCT_NAME = {
    .pin_desc  = &pwm_pin_desc[0],
    .base_addr = PWM_BASE_ADDR,
    .clk_name  = PWM_CLK_NAME(),
    .clk_div   = OPTION_GET(NUMBER,clk_div),
    .idx       = PWM_DEV_ID,
    .comp_mask = PWM_COMP_MASK,
};

PWM_DEV_DEF(PWM_DEV_ID, &niiet_pwm_ops, &PWM_DEV_PRIV_STRUCT_NAME,
                        &pwm_pin_desc[0], PWM_BASE_ADDR,
                        PWM_AVAIL_MASK, NIIET_PWM_CHAN_MAX,
                    PWM_DMAS);