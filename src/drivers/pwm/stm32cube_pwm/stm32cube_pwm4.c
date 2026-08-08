/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>

#include <util/macro.h>

#include <kernel/irq.h>

#include <config/board_config.h>
#include <drivers/gpio.h>

#include <drivers/pwm.h>
#include <drivers/pin_description.h>

#include <framework/mod/options.h>

#include "stm32cube_pwm_priv.h"

#define PWM_DEV_ID                4
#define PWM_DEV_PRIV_STRUCT_NAME  MACRO_CONCAT(pwm_dev_priv, PWM_DEV_ID)

#define CONF_PWM                  MACRO_CONCAT(CONF_PWM,PWM_DEV_ID)

#if defined CONF_PWM4_ENABLED

#define PWM_BASE_ADDR        MACRO_CONCAT(CONF_PWM,_REGION_BASE_ADDR)
#define PWM_CLK_NAME         MACRO_CONCAT(CONF_PWM,_CLK_ENABLE)
#define PWM_CHANNEL_NR       MACRO_CONCAT(CONF_PWM,_CHANNEL)
#define PWM_COMP_MASK        MACRO_CONCAT(CONF_PWM,_MISC_COMP_MASK)

#if defined(CONF_PWM4_PIN_OUT0_PORT)
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

#if defined(CONF_PWM4_PIN_OUT1_PORT)
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

#if defined(CONF_PWM4_PIN_OUT2_PORT)
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

#if defined(CONF_PWM4_PIN_OUT3_PORT)
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

#if defined(CONF_PWM4_DMA_OUT0_NUM)
#define DMA0_EN     1
#define DMA0_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT0_NUM)
#define DMA0_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT0_CHAN)
#define DMA0_STREAM MACRO_CONCAT(CONF_PWM, _DMA_OUT0_STREAM)
#define DMA0_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT0)
#else
#define DMA0_EN     0
#define DMA0_NUM    0
#define DMA0_CHAN   0
#define DMA0_STREAM 0
#endif

#if defined(CONF_PWM4_DMA_OUT1_NUM)
#define DMA1_EN     1
#define DMA1_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT1_NUM)
#define DMA1_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT1_CHAN)
#define DMA1_STREAM MACRO_CONCAT(CONF_PWM, _DMA_OUT1_STREAM)
#define DMA1_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT1)
#else
#define DMA1_EN     0
#define DMA1_NUM    0
#define DMA1_CHAN   0
#define DMA1_STREAM 0
#endif

#if defined(CONF_PWM4_DMA_OUT2_NUM)
#define DMA2_EN     1
#define DMA2_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT2_NUM)
#define DMA2_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT2_CHAN)
#define DMA2_STREAM MACRO_CONCAT(CONF_PWM, _DMA_OUT2_STREAM)
#define DMA2_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT2)
#else
#define DMA2_EN     0
#define DMA2_NUM    0
#define DMA2_CHAN   0
#define DMA2_STREAM 0
#endif

#if defined(CONF_PWM4_DMA_OUT3_NUM)
#define DMA3_EN     1
#define DMA3_NUM    MACRO_CONCAT(CONF_PWM, _DMA_OUT3_NUM)
#define DMA3_CHAN   MACRO_CONCAT(CONF_PWM, _DMA_OUT3_CHAN)
#define DMA3_STREAM MACRO_CONCAT(CONF_PWM, _DMA_OUT3_STREAM)
#define DMA3_IRQ    MACRO_CONCAT(CONF_PWM, _IRQ_DMA_OUT3)
#else
#define DMA3_EN     0
#define DMA3_NUM    0
#define DMA3_CHAN   0
#define DMA3_STREAM 0
#endif

#endif /* defined CONF_PWM4_ENABLED */

#if DMA0_EN || DMA1_EN || DMA2_EN || DMA3_EN

static uint32_t pwm_dmas[STM_PWM_CHAN_MAX] = {
    (DMA0_EN << 31) | (DMA0_NUM << 0) | (DMA0_CHAN << 8) | (DMA0_STREAM << 16),
    (DMA1_EN << 31) | (DMA1_NUM << 0) | (DMA1_CHAN << 8) | (DMA1_STREAM << 16),
    (DMA2_EN << 31) | (DMA2_NUM << 0) | (DMA2_CHAN << 8) | (DMA2_STREAM << 16),
    (DMA3_EN << 31) | (DMA3_NUM << 0) | (DMA3_CHAN << 8) | (DMA3_STREAM << 16),
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

static const struct pin_description pwm_pin_desc[STM_PWM_CHAN_MAX] = {
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

extern irq_return_t stm32cube_pwm_dma_irq_handler(unsigned int irq_num, void *priv);
extern struct pwm_ops stm32cube_pwm_ops;

static struct stm32cube_pwm_priv PWM_DEV_PRIV_STRUCT_NAME = {
    .pin_desc  = &pwm_pin_desc[0],
    .base_addr = PWM_BASE_ADDR,
    .clk_name  = PWM_CLK_NAME(),
    .tim_freq  = OPTION_GET(NUMBER,tim_freq),
    .idx       = PWM_DEV_ID,
    .comp_mask = PWM_COMP_MASK,
};

#if DMA0_EN
STATIC_IRQ_ATTACH(DMA0_IRQ, stm32cube_pwm_dma_irq_handler, &PWM_DEV_PRIV_STRUCT_NAME);
#endif /* DMA0_EN */

#if DMA1_EN
STATIC_IRQ_ATTACH(DMA1_IRQ, stm32cube_pwm_dma_irq_handler, &PWM_DEV_PRIV_STRUCT_NAME);
#endif /* DMA1_EN */

#if DMA2_EN
STATIC_IRQ_ATTACH(DMA2_IRQ, stm32cube_pwm_dma_irq_handler, &PWM_DEV_PRIV_STRUCT_NAME);
#endif /* DMA20_EN */

#if DMA3_EN
STATIC_IRQ_ATTACH(DMA3_IRQ, stm32cube_pwm_dma_irq_handler, &PWM_DEV_PRIV_STRUCT_NAME);
#endif /* DMA3_EN */

PWM_DEV_DEF(PWM_DEV_ID, &stm32cube_pwm_ops, &PWM_DEV_PRIV_STRUCT_NAME,
                        &pwm_pin_desc[0], PWM_BASE_ADDR,
                        PWM_AVAIL_MASK, STM_PWM_CHAN_MAX,
                    PWM_DMAS);