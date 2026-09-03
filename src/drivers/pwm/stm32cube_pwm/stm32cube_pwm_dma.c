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
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <hal/system.h>

#include <kernel/time/time.h>

#include <drivers/pwm.h>
#include <drivers/pin_description.h>
#include <drivers/gpio.h>
#include <drivers/clk.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include "stm32cube_pwm_priv.h"

extern int clk_enable(char *clk_name);

static inline void *stm32cube_dma_get_inst(int dma, int stream) {
	switch (dma) {
	case 1:
		switch (stream) {
		case 0:
			return (void *)(uintptr_t)DMA1_Stream0_BASE;
		case 1:
			return (void *)(uintptr_t)DMA1_Stream1_BASE;
		case 2:
			return (void *)(uintptr_t)DMA1_Stream2_BASE;
		case 3:
			return (void *)(uintptr_t)DMA1_Stream3_BASE;
		case 4:
			return (void *)(uintptr_t)DMA1_Stream4_BASE;
		case 5:
			return (void *)(uintptr_t)DMA1_Stream5_BASE;
		case 6:
			return (void *)(uintptr_t)DMA1_Stream6_BASE;
		case 7:
			return (void *)(uintptr_t)DMA1_Stream7_BASE;
		default:
			return NULL;
		}
	case 2:
		switch (stream) {
		case 0:
			return (void *)(uintptr_t)DMA2_Stream0_BASE;
		case 1:
			return (void *)(uintptr_t)DMA2_Stream1_BASE;
		case 2:
			return (void *)(uintptr_t)DMA2_Stream2_BASE;
		case 3:
			return (void *)(uintptr_t)DMA2_Stream3_BASE;
		case 4:
			return (void *)(uintptr_t)DMA2_Stream4_BASE;
		case 5:
			return (void *)(uintptr_t)DMA2_Stream5_BASE;
		case 6:
			return (void *)(uintptr_t)DMA2_Stream6_BASE;
		case 7:
			return (void *)(uintptr_t)DMA2_Stream7_BASE;
		default:
			return NULL;
		}
	default:
		return NULL;
	}
	return NULL;
}

static inline int stm32cube_dma_get_channel(int chan) {
	switch (chan) {
	case 0:
		return DMA_CHANNEL_0;
	case 1:
		return DMA_CHANNEL_1;
	case 2:
		return DMA_CHANNEL_2;
	case 3:
		return DMA_CHANNEL_3;
	case 4:
		return DMA_CHANNEL_4;
	case 5:
		return DMA_CHANNEL_5;
	case 6:
		return DMA_CHANNEL_6;
	case 7:
		return DMA_CHANNEL_7;

	default:
		return -1;
	}
	return -1;
}

static inline int stm32cube_dma_clock_en(int dma) {
	switch (dma) {
	case 1:
        clk_enable("CLK_DMA1");
        return 0;
    case 2:
        clk_enable("CLK_DMA2");
    return 0;
        
    default:
        return -1;
    }
    return -1;
}

int stm32cube_pwm_dma_init(struct pwm_device *dev, int i) {
    struct stm32cube_pwm_priv *priv;
    void *instance;
    uint32_t chan;

    priv = dev->pwmd_priv;

    stm32cube_dma_clock_en(STM32CUBE_PWM_DMA_NUM(dev->pwmd_dma[i]));

    instance = stm32cube_dma_get_inst(STM32CUBE_PWM_DMA_NUM(dev->pwmd_dma[i]),
                    STM32CUBE_PWM_DMA_STREAM(dev->pwmd_dma[i]));
    if (instance == NULL) {
        return 0;
    }

    chan = stm32cube_dma_get_channel(STM32CUBE_PWM_DMA_CHAN(dev->pwmd_dma[i]));
    if (chan == -1) {
        return 0;
    }

    log_debug("En DMA%d_Stream%d Chan%d\n\tinst(0x%x) chan(0x%x)",
                        STM32CUBE_PWM_DMA_NUM(dev->pwmd_dma[i]), 
                        STM32CUBE_PWM_DMA_STREAM(dev->pwmd_dma[i]),
                        STM32CUBE_PWM_DMA_CHAN(dev->pwmd_dma[i]),
                        instance, chan );

    priv->dma_handle[i].Instance = instance;
    priv->dma_handle[i].Init.Channel = chan;
    priv->dma_handle[i].Init.Direction = DMA_MEMORY_TO_PERIPH;
    priv->dma_handle[i].Init.PeriphInc = DMA_PINC_DISABLE;
    priv->dma_handle[i].Init.MemInc = DMA_MINC_ENABLE;
    priv->dma_handle[i].Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    priv->dma_handle[i].Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    priv->dma_handle[i].Init.Mode = DMA_NORMAL;
    priv->dma_handle[i].Init.Priority = DMA_PRIORITY_HIGH;
    priv->dma_handle[i].Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    priv->dma_handle[i].Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
    priv->dma_handle[i].Init.MemBurst = DMA_MBURST_SINGLE;
    priv->dma_handle[i].Init.PeriphBurst = DMA_PBURST_SINGLE;

    return 0;
}

