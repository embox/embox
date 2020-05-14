/**
 * @file
 * @brief
 *
 * @date 10.05.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <util/log.h>

#include <config/custom_config_qspi.h>

#include <hw_cpm.h>
#include <hw_gpio.h>
#include <hw_usb.h>

#include <sys_clock_mgr.h>
#include <sys_clock_mgr_internal.h>

#include <third_party/dialog/da14695/usb_da1469x.h>

#define USB_IRQ           31
#define VBUS_IRQ          37

static_assert(USB_IRQ == USB_IRQn + 16);
static_assert(VBUS_IRQ == VBUS_IRQn + 16);

static int usb_da1469x_reset_hnd(struct lthread *self);
static LTHREAD_DEF(usb_da1469x_reset_lt, usb_da1469x_reset_hnd, 200);

static int usb_da1469x_reset_hnd(struct lthread *self) {
	cm_sys_clk_set(sysclk_PLL96);
	hw_usb_reset_event();
	return 0;
}

static void usb_da1469x_usb_irq_cb(uint32_t status) {
	//log_debug("status = 0x%08x", status);

	if (status & USB_USB_MAEV_REG_USB_ALT_Msk) {
		uint8_t altev;

		altev = USB->USB_ALTEV_REG;
		altev &= USB->USB_ALTMSK_REG;

		if (altev & USB_USB_ALTEV_REG_USB_RESET_Msk) {
			REG_CLR_BIT(USB, USB_ALTMSK_REG, USB_M_RESET);
			lthread_launch(&usb_da1469x_reset_lt);
		}
	}

	if (status & USB_USB_MAEV_REG_USB_FRAME_Msk) {
		hw_usb_frame_event();
	}
	
	if (status & USB_USB_MAEV_REG_USB_NAK_Msk) {
		hw_usb_nak_event();
	}
	
	if (status & USB_USB_MAEV_REG_USB_TX_EV_Msk) {
		hw_usb_tx_event();
	}
	
	if (status & USB_USB_MAEV_REG_USB_RX_EV_Msk) {
		hw_usb_rx_event();
	}
	
	if (status & USB_USB_MAEV_REG_USB_EP0_NAK_Msk) {
		hw_usb_nak_event_ep0();
	}
	
	if (status & USB_USB_MAEV_REG_USB_EP0_TX_Msk) {
		hw_usb_tx_ep(0);
	}
	
	if (status & USB_USB_MAEV_REG_USB_EP0_RX_Msk) {
		hw_usb_rx_ep0();
	}
}

extern void USB_Handler(void);
static irq_return_t usb_da1469x_usb_irq_handler(unsigned int irq_nr,
		void *data) {
	USB_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(USB_IRQ, usb_da1469x_usb_irq_handler, NULL);

/* FIXME */
extern void sys_usb_ext_hook_begin_enumeration(void);

static void usb_da1469x_attach(void) {
	int ret;

	hw_usb_enable_usb_interrupt(usb_da1469x_usb_irq_cb);

	ret = irq_attach(USB_IRQ, usb_da1469x_usb_irq_handler, 0,
	                 NULL, "usb da1469x");
	if (ret != 0) {
		log_error("USB irq attach failed");
	}

	hw_usb_init();
	GPREG->USBPAD_REG = 0;
	
	//   Power up USB hardware.
	USB->USB_MCTRL_REG = USB_USB_MCTRL_REG_USBEN_Msk;
	USB->USB_MCTRL_REG = USB_USB_MCTRL_REG_USBEN_Msk | USB_USB_MCTRL_REG_USB_NAT_Msk;
	
	// Now that everything is ready, announce device presence to the USB host.
	hw_usb_bus_attach();

	sys_usb_ext_hook_begin_enumeration();
}

static void usb_da1469x_vbus_handler(void) {
	if (CRG_TOP->ANA_STATUS_REG & REG_MSK(CRG_TOP, ANA_STATUS_REG, VBUS_AVAILABLE)) {
		log_debug("VBUS is available!");
		if (hw_usb_is_powered_by_vbus()) {
			usb_da1469x_attach();
		}
	}
}

static void usb_da1469x_hw_vbus_irq_handler(HW_USB_VBUS_IRQ_STAT status) {
	(void) status;
	log_error("NIY");
}

extern void VBUS_Handler(void);
static irq_return_t usb_da1469x_vbus_irq_handler(unsigned int irq_nr,
		void *data) {
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

void usb_da1469x_clocks_init(void) {
	cm_sys_clk_init(sysclk_XTAL32M);
	cm_apb_set_clock_divider(apb_div1);
	cm_ahb_set_clock_divider(ahb_div1);

	cm_sys_clk_set(sysclk_XTAL32M);
}

int usb_da1469x_init(void) {
	int ret = 0;

	log_debug("");

	usb_da1469x_clocks_init();

	usb_da1469x_pins_configure();

	hw_usb_enable_vbus_interrupt(usb_da1469x_hw_vbus_irq_handler);

	ret = irq_attach(VBUS_IRQ, usb_da1469x_vbus_irq_handler, 0,
	                 NULL, "vbus da1469x");
	if (ret != 0) {
		log_error("VBUS irq attach failed");
		return ret;
	}

	hw_usb_program_vbus_irq_on_rising();
	hw_usb_program_vbus_irq_on_falling();

	if (hw_usb_is_powered_by_vbus()) {
		usb_da1469x_vbus_handler();
	} else {
		log_error("VBUS is not powered! Exit.");
		return -1;
	}

	return 0;
}
