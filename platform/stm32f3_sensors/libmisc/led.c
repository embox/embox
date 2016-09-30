/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#include "led.h"

void led_init(Led_TypeDef led) {
	BSP_LED_Init(led);
}

void led_on(Led_TypeDef led) {
	BSP_LED_On(led);
}

void led_off(Led_TypeDef led) {
	BSP_LED_Off(led);
}
