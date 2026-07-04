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

#include <embox/unit.h>

#include <util/math.h>

#include <config/board_config.h>

#include "k1921vg015_udc_priv.h"

EMBOX_UNIT_INIT(niiet_udc_init);

#define USB_IRQ_NUM       CONF_USB_IRQ

#define NIIET_UDC_IN_EP_MASK  ((1 << 1) | (1 << 2) | (1 << 3))
#define NIIET_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))


static irq_return_t niiet_usbd_irq_handler(unsigned int irq_nr, void *data) {

	if (log_level_self() >= LOG_DEBUG) {
		printk("usb: irq entry\n");
	}

	return IRQ_HANDLED;
}

static int niiet_udc_start(struct usb_udc *udc) {

	return 0;
}

static int niiet_udc_ep_queue(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {

	return 0;
}

static void niiet_udc_ep_enable(struct usb_gadget_ep *ep) {

}

static struct usb_udc_ops niiet_usb_udc_ops = {
	.uuo_udc_start = niiet_udc_start,
	.uuo_ep_queue = niiet_udc_ep_queue,
	.uuo_ep_enable = niiet_udc_ep_enable,
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

static int niiet_udc_init(void) {
	int ret;

	ret = irq_attach(USB_IRQ_NUM, niiet_usbd_irq_handler, 0, &niiet_udc.hpcd, "udc");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	usb_gadget_udc_register(&niiet_udc.udc);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ_NUM, niiet_usbd_irq_handler, &niiet_udc);