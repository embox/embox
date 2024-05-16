/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    27.01.2020
 */

#include <bsp/stm32cube_hal.h>
#include <hal/platform.h>

extern void SystemClock_Config(void);

void platform_init() {
	SystemInit();
	HAL_Init();
	SystemClock_Config();
}
