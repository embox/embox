/**
 * @file
 * @brief
 *
 * @date 26.04.20
 * @author Alexander Kalmuk
 */

#ifndef DRIVERS_INPUT_TOUCHSCREEN_STM32_STM32F769I_TS__H_
#define DRIVERS_INPUT_TOUCHSCREEN_STM32_STM32F769I_TS__H_

#include "stm32f769i_discovery_ts.h"

#define STM32_TS_INT_PIN   TS_INT_PIN
#define STM32_TS_IRQ       (TS_INT_EXTI_IRQn + 16)

#endif /* DRIVERS_INPUT_TOUCHSCREEN_STM32_STM32F769I_TS__H_ */
