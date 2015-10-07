/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#include "button.h"

void button_init(Button_TypeDef button) {
	BSP_PB_Init(button, BUTTON_MODE_GPIO);
}

void button_wait_set(Button_TypeDef button) {
	while(BSP_PB_GetState(button) != SET) 
		;
}

void button_wait_reset(Button_TypeDef button) {
	while(BSP_PB_GetState(button) != RESET) 
		;
}
