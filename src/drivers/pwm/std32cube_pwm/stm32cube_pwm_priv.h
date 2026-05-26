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

#include <drivers/pwm.h> /* only for struct pin_description  */

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>


#define STM_PWM_CHAN_MAX    (4)

struct stm32cube_pwm_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	int                           tim_freq;
	int                           idx;
	uint32_t                      comp_mask;

	TIM_HandleTypeDef             TimHandle;
	TIM_OC_InitTypeDef            sConfig;
};

#endif /* DRIVERS_PWM_STM32CUBE_PWM_PRIV_H_ */
