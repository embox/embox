#include <assert.h>
#include <kernel/irq.h>
#include <framework/mod/options.h>
#include <DA1469xAB.h>

#define CMAC2SYS_IRQ      OPTION_GET(NUMBER, cmac2sys_irq)
static_assert(CMAC2SYS_IRQ == CMAC2SYS_IRQn);

extern void CMAC2SYS_Handler(void);
static irq_return_t cmac2sys_irq_handler(unsigned int irq_nr,
		void *data) {
	CMAC2SYS_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(CMAC2SYS_IRQ, cmac2sys_irq_handler, NULL);

int os_arch_main_init(void) {
	return 0;
}
