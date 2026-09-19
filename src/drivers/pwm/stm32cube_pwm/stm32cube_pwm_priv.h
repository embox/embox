/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_PWM_STM32CUBE_PWM_PRIV_H_
#define DRIVERS_PWM_STM32CUBE_PWM_PRIV_H_

#include <stdint.h>

#include <kernel/irq.h>

#include <drivers/pwm.h> /* only for struct pin_description  */

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#define STM32CUBE_PWM_DMA_BUF_SIZE    (32)

#define STM_PWM_CHAN_MAX              (4)

#define STM32CUBE_PWM_WRONG_CHAN  (0xFFFFFFF)

#define STM32CUBE_PWM_DMA_NUM_OFF     (0)
#define STM32CUBE_PWM_DMA_STREAM_OFF  (8)
#define STM32CUBE_PWM_DMA_CHAN_OFF    (16)
#define STM32CUBE_PWM_DMA_EN_OFF      (31)

#define STM32CUBE_PWM_DMA_EN(desc)    \
				(desc & (1 << STM32CUBE_PWM_DMA_EN_OFF))

#define STM32CUBE_PWM_DMA_NUM(desc)    \
				((desc >> STM32CUBE_PWM_DMA_NUM_OFF) & 0xFF)
#define STM32CUBE_PWM_DMA_CHAN(desc)   \
				((desc >> STM32CUBE_PWM_DMA_CHAN_OFF) & 0xFF)
#define STM32CUBE_PWM_DMA_STREAM(desc) \
				((desc >> STM32CUBE_PWM_DMA_STREAM_OFF) & 0xFF)

struct stm32cube_pwm_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	int                           tim_freq;
	int                           idx;
	uint32_t                      comp_mask;

	TIM_HandleTypeDef             TimHandle;
	TIM_OC_InitTypeDef            sConfig;
	DMA_HandleTypeDef             dma_handle[STM_PWM_CHAN_MAX];
	uint32_t                      dma_buffer[STM_PWM_CHAN_MAX][STM32CUBE_PWM_DMA_BUF_SIZE];
};

extern irq_return_t stm32cube_pwm_dma_irq_handler(unsigned int irq_num, void *priv) ;

#endif /* DRIVERS_PWM_STM32CUBE_PWM_PRIV_H_ */
