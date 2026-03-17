/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2014
 */

#include <bsp/stm32cube_hal.h>

extern void stm32_sysclock_init(void);

void SystemClock_Config(void) {
	stm32_sysclock_init();
}
