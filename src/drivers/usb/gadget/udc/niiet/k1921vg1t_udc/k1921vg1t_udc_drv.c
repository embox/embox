/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#include <util/log.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/gadget.h>

#include <kernel/printk.h>
#include <kernel/irq.h>
#include <lib/libds/array.h>

#include <util/macro.h>

#include <embox/unit.h>

#include <util/math.h>

#include <drivers/clk/niiet_rcu.h>

#include <config/board_config.h>

#include "k1921vg1t_udc_priv.h"

EMBOX_UNIT_INIT(niiet_udc_init);

#define USB_NUM           OPTION_GET(NUMBER, usb_num)


#define CONF_USB_IRQ          MACRO_CONCAT(MACRO_CONCAT(CONF_USB,USB_NUM),_IRQ)
#define CONF_USB_REGION_BASE  MACRO_CONCAT(MACRO_CONCAT(CONF_USB,USB_NUM),_REGION_BASE)
#define CONF_USB_CLK_DEF_USB  MACRO_CONCAT(MACRO_CONCAT(CONF_USB,USB_NUM),_CLK_DEF_USB)
#define CONF_USB_MISC_EP_MAX_SIZE MACRO_CONCAT(MACRO_CONCAT(CONF_USB,USB_NUM),_MISC_EP_MAX_SIZE)

#define USB_IRQ_NUM           CONF_USB_IRQ

#define NIIET_UDC_IN_EP_MASK  ((1 << 1) | (1 << 2) | (1 << 3))
#define NIIET_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))

#define USB      ((struct niiet_usbd_regs *)(uintptr_t) CONF_USB_REGION_BASE)

static inline void usb_control_header_show(struct usb_control_header *ctrl) {
	if (log_level_self() >= LOG_DEBUG) {
		printk("usb_control_header:\n");
		printk("\tbm_request_type: %x\n", ctrl->bm_request_type);
		printk("\tb_request: %x\n", ctrl->b_request);
		printk("\tw_value: %x\n", ctrl->w_value);
		printk("\tw_index: %x\n", ctrl->w_index);
		printk("\tw_index: %x\n", ctrl->w_length);
	}

}

static irq_return_t niiet_usbd_irq_handler(unsigned int irq_nr, void *data) {


	return IRQ_HANDLED;
}


static int niiet_udc_start(struct usb_udc *udc) {
	clk_enable(CONF_USB_CLK_DEF_USB);


	return 0;
}

static int niiet_udc_ep_queue(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {

	return 0;
}

static int niiet_udc_ep_stall(struct usb_gadget_ep *ep,
    const struct usb_control_header *ctrl)  {
	return 0;
}

static void niiet_udc_ep_enable(struct usb_gadget_ep *ep) {

}

static int niiet_udc_set_addr(struct usb_udc *udc, uint8_t addr) {
	//struct niiet_udc *niiet_udc = member_cast_out(udc, struct niiet_udc, udc);

	log_debug("niiet_udc_set_addr: addr=%u", addr);

	return 0;
}

static struct usb_udc_ops niiet_usb_udc_ops = {
	.uuo_udc_start = niiet_udc_start,
	.uuo_ep_queue = niiet_udc_ep_queue,
	.uuo_ep_stall = niiet_udc_ep_stall,
	.uuo_ep_enable = niiet_udc_ep_enable,
	.uuo_set_addr = niiet_udc_set_addr,
};

struct niiet_udc niiet_udc = {
    .udc =
        {
            .udc_name = "udc",
            .udc_ops = &niiet_usb_udc_ops,

            .udc_in_ep_mask = NIIET_UDC_IN_EP_MASK,
            .udc_out_ep_mask = NIIET_UDC_OUT_EP_MASK,

			.udc_ep0_max_size = USB_MAX_EP0_SIZE,
			.udc_ep_max_size = CONF_USB_MISC_EP_MAX_SIZE,
        },
};

extern void usb_phy_utmi_bconf_init(void) ;

static int niiet_udc_init(void) {
	int ret;

	ret = irq_attach(USB_IRQ_NUM, niiet_usbd_irq_handler, 0, &niiet_udc, "udc");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	usb_phy_utmi_bconf_init();

	usb_gadget_udc_register(&niiet_udc.udc);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ_NUM, niiet_usbd_irq_handler, &niiet_udc);