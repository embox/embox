/**
 * @file
 * @brief
 *
 * @author  Filipp Chubukov
 * @date    04.05.2021
 */
#ifndef SRC_DRIVERS_USB_STM32_HC_STM32_HC3_H_
#define SRC_DRIVERS_USB_STM32_HC_STM32_HC3_H_

#if defined(STM32F429xx) || defined(STM32F407xx)

#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_hcd.h>

#else
#error "don't support platform"
#endif

#if defined(STM32F429xx)
#define STM32_HC_VBUS_PORT            GPIOG
#define STM32_HC_VBUS_PIN             GPIO_PIN_6
#define STM32_HC_PORT_VBUS_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE
#elif defined(STM32F407xx)
#define STM32_HC_VBUS_PORT            GPIOC
#define STM32_HC_VBUS_PIN             GPIO_PIN_0
#define STM32_HC_PORT_VBUS_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE
#else
#error "don't support platform"
#endif

#endif /* SRC_DRIVERS_USB_STM32_HC_STM32_HC3_H_ */
