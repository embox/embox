/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_
#define PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_

#include <stm32f3xx.h>
#include <stm32f3_discovery.h>

extern void led_init(Led_TypeDef led);
extern void led_on(Led_TypeDef led);
extern void led_off(Led_TypeDef led);

#endif /* PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_ */

