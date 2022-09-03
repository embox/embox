/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_
#define PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_

#include <stdint.h>

struct led {
	uint16_t port;
	uint16_t pin;
};

extern void led_init(struct led *led, int port, int pin);
extern void led_on(struct led *led);
extern void led_off(struct led *led);

#include <drivers/gpio/gpio.h>

#define STM_LED6_PIN                         15
#define STM_LED6_GPIO_PORT                   GPIO_PORT_E

#define STM_LED8_PIN                         14
#define STM_LED8_GPIO_PORT                   GPIO_PORT_E

#define STM_LED10_PIN                        13
#define STM_LED10_GPIO_PORT                  GPIO_PORT_E

#define STM_LED9_PIN                         12
#define STM_LED9_GPIO_PORT                   GPIO_PORT_E

#define STM_LED7_PIN                         11
#define STM_LED7_GPIO_PORT                   GPIO_PORT_E

#define STM_LED5_PIN                         10
#define STM_LED5_GPIO_PORT                   GPIO_PORT_E

#define STM_LED3_PIN                         9
#define STM_LED3_GPIO_PORT                   GPIO_PORT_E

#define STM_LED4_PIN                         8
#define STM_LED4_GPIO_PORT                   GPIO_PORT_E

#endif /* PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_ */

