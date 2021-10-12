/**
 * @file
 *
 * @date Oct 9, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_USB_STM32_HC_STM32_HC_CONF_H7_H_
#define SRC_DRIVERS_USB_STM32_HC_STM32_HC_CONF_H7_H_

#include <stm32h7xx.h>
#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_hcd.h>

/* for STM32H745i-discovery */
#define STM32_HC_VBUS_PORT            GPIOA
#define STM32_HC_VBUS_PIN             GPIO_PIN_5
#define STM32_HC_PORT_VBUS_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE

#endif /* SRC_DRIVERS_USB_STM32_HC_STM32_HC_CONF_H7_H_ */
