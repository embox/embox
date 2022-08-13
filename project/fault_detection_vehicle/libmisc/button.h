/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBMISC_BUTTON_H_
#define PLATFORM_STM32F3_SENSORS_LIBMISC_BUTTON_H_

#include <stm32f3xx.h>
#include <stm32f3_discovery.h>

extern void button_init(Button_TypeDef button);
extern void button_wait_set(Button_TypeDef button);
extern void button_wait_reset(Button_TypeDef button);

#endif /* PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_ */

