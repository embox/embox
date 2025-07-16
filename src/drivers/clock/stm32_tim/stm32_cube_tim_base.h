/**
 * @file
 *
 * @date   12.11.2020
 * @author Alexander Kalmuk
 */

#ifndef SRC_DRIVERS_CLOCK_STM32_CUBE_TIM_BASE_H_
#define SRC_DRIVERS_CLOCK_STM32_CUBE_TIM_BASE_H_

#include <stm32f7xx_hal.h>

extern int stm32_cube_tim_base_set_oneshot(struct clock_source *cs);
extern int stm32_cube_tim_base_set_periodic(struct clock_source *cs);
extern int stm32_cube_tim_base_set_next_event(struct clock_source *cs,
		uint32_t next_event);
extern cycle_t stm32_cube_tim_get_cycles(struct clock_source *cs);
extern int stm32_cube_tim_base_config(struct clock_source *cs);
extern irq_return_t stm32_cube_time_base_irq_handler(unsigned int irq_nr,
		void *data);

#endif /* SRC_DRIVERS_CLOCK_STM32_CUBE_TIM_BASE_H_ */
