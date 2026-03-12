/**
 * @file
 * @brief
 *
 * @date 12.03.2026
 * @author Anton Bondarev
 */

#ifndef STM32BSP_TOUCHSCREEN_F4_H_
#define STM32BSP_TOUCHSCREEN_F4_H_

#include "stm32f429i_discovery_ts.h"

#define STM32_TS_INT_PIN   STMPE811_INT_PIN
#define STM32_TS_IRQ       OPTION_GET(NUMBER, ts_irq)
static_assert(STM32_TS_IRQ == STMPE811_INT_EXTI, "");


#define TOUCH_STATE_TYPE      TS_StateTypeDef

#define TOUCH_IRQ_CONFIG()    BSP_TS_ITConfig()
#define TOUCH_GETSTATE(ts)    BSP_TS_GetState(ts) 
#define TOUCH_DETECTED(ts)    (ts.TouchDetected)
#define TOUCH_X(ts, i)        (ts.X)
#define TOUCH_Y(ts, i)        (ts.Y)

#define TOUCH_IRQ_HANDLE() \
    		BSP_TS_ITClear();                          \
    		__HAL_GPIO_EXTI_CLEAR_IT(STM32_TS_INT_PIN)

static inline int stm32bsp_touch_init(uint32_t xres, uint32_t yres) {
	return BSP_TS_Init(xres, yres);
}

#endif /* STM32BSP_TOUCHSCREEN_F4_H_ */
