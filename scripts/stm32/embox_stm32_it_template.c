/**
 * GENERATED FILE
 */
#include <kernel/irq.h>

#include "main.h"
#include "stm32_PLATFORM_xx_it.h"

#define EMBOX_STM32_IRQ_HANDLER(prefix) \
	static irq_return_t embox_##prefix##_IRQHandler(unsigned int irq_nr, void *data) { \
		prefix##_IRQHandler(); \
		return IRQ_HANDLED; \
	}

_IRQ_HANDLERS_DECLARE_

extern void (*cortexm_external_clock_hnd)(void);

int embox_stm32_setup_irq_handlers(void) {
	int res = 0;

	cortexm_external_clock_hnd = SysTick_Handler;

_STM32_IRQ_ATTACH_

	return res;
}
