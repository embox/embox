/**
 * @file
 * @brief
 *
 * @date 12.03.2026
 * @author Anton Bondarev
 */

#ifndef STM32BSP_TOUCHSCREEN_F7_H_
#define STM32BSP_TOUCHSCREEN_F7_H_


#if defined STM32F746xx
#include "stm32746g_discovery_ts.h"
#elif defined STM32F769xx
#include "stm32f769i_discovery_ts.h"
#else
#error Unsupported platform
#endif

#define STM32_TS_INT_PIN   TS_INT_PIN
#define STM32_TS_INT_PORT  TS_INT_GPIO_PORT
#define STM32_TS_IRQ       OPTION_GET(NUMBER, ts_irq)
static_assert(STM32_TS_IRQ == TS_INT_EXTI_IRQn, "");

#define TOUCH_STATE_TYPE      TS_StateTypeDef

#define TOUCH_IRQ_CONFIG()    BSP_TS_ITConfig()
#define TOUCH_GETSTATE(ts)    BSP_TS_GetState(ts) 
#define TOUCH_DETECTED(ts)    (ts.touchDetected)
#define TOUCH_X(ts, i)        (ts.touchX[i])
#define TOUCH_Y(ts, i)        (ts.touchY[i])

#define TOUCH_IRQ_HANDLE() \
			__HAL_GPIO_EXTI_CLEAR_IT(STM32_TS_INT_PIN)

static inline int stm32bsp_touch_init(uint32_t xres, uint32_t yres) {
	return BSP_TS_Init(xres, yres);
}

#endif /* STM32BSP_TOUCHSCREEN_F7_H_ */
