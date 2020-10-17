/**
 * @file
 * @brief
 *
 * @date 10.05.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <kernel/irq.h>
#include <util/log.h>
#include <framework/mod/options.h>

#include <config/custom_config_qspi.h>

#include <hw_cpm.h>
#include <hw_gpio.h>
#include <hw_usb.h>
#include <sys_clock_mgr.h>
#include <sys_clock_mgr_internal.h>
#include <sys_usb_da1469x.h>

#include "usb_da1469x.h"

#define USB_IRQ           OPTION_GET(NUMBER, usb_irq)
#define VBUS_IRQ          OPTION_GET(NUMBER, vbus_irq)

static_assert(USB_IRQ == USB_IRQn);
static_assert(VBUS_IRQ == VBUS_IRQn);

extern void USB_Handler(void);
static irq_return_t usb_da1469x_usb_irq_handler(unsigned int irq_nr,
		void *data) {
	USB_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(USB_IRQ, usb_da1469x_usb_irq_handler, NULL);

extern void arch_deepsleep_disable(uint32_t mask);
#define DEEPSLEEP_DISABLE_USB     0x1

extern void VBUS_Handler(void);
static irq_return_t usb_da1469x_vbus_irq_handler(unsigned int irq_nr,
		void *data) {
	arch_deepsleep_disable(DEEPSLEEP_DISABLE_USB);
	VBUS_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(VBUS_IRQ, usb_da1469x_vbus_irq_handler, NULL);

static void usb_da1469x_pins_configure(void) {
	hw_gpio_reserve_and_configure_pin(HW_GPIO_PORT_0, HW_GPIO_PIN_14,
	                                  HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_USB, true);
	hw_gpio_reserve_and_configure_pin(HW_GPIO_PORT_0, HW_GPIO_PIN_15,
	                                  HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_USB, true);
	hw_gpio_pad_latch_enable(HW_GPIO_PORT_0, HW_GPIO_PIN_14);
	hw_gpio_pad_latch_enable(HW_GPIO_PORT_0, HW_GPIO_PIN_15);
}

static void usb_da1469x_clocks_init(void) {
	cm_sys_clk_init(sysclk_XTAL32M);
	cm_apb_set_clock_divider(apb_div1);
	cm_ahb_set_clock_divider(ahb_div1);

	cm_sys_clk_set(sysclk_XTAL32M);
}

int usb_da1469x_init(void) {
	log_debug("");

	usb_da1469x_clocks_init();
	usb_da1469x_pins_configure();

	sys_usb_init();

	return 0;
}
