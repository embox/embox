/**
 * @file
 * @brief
 *
 * @date 12.03.2026
 * @author Anton Bondarev
 */

#ifndef STM32BSP_TOUCHSCREEN_H7_H_
#define STM32BSP_TOUCHSCREEN_H7_H_

#include "stm32h745i_discovery_ts.h"

#define STM32_TS_INT_PIN   TS_INT_PIN
#define STM32_TS_IRQ       OPTION_GET(NUMBER, ts_irq)
static_assert(STM32_TS_IRQ == TS_INT_EXTI_IRQn, "");

#define TOUCH_STATE_TYPE      TS_State_t

#define TOUCH_IRQ_CONFIG()    BSP_TS_EnableIT(0)
#define TOUCH_GETSTATE(ts)    BSP_TS_GetState(0, ts) 
#define TOUCH_DETECTED(ts)    (ts.TouchDetected)
#define TOUCH_X(ts, i)        (ts.TouchX)
#define TOUCH_Y(ts, i)        (ts.TouchY)

#define TOUCH_IRQ_HANDLE() \
		BSP_TS_IRQHandler(0)

static inline int stm32bsp_touch_init(uint32_t xres, uint32_t yres) {
	TS_Init_t   hTS;

	hTS.Width = xres;
	hTS.Height = yres;
	hTS.Orientation = TS_SWAP_XY ;
	hTS.Accuracy = 5;

	return BSP_TS_Init(0, &hTS);
}

#endif /* STM32BSP_TOUCHSCREEN_H7_H_ */
