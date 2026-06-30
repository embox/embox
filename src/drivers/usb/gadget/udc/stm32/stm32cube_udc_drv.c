/**
 * @file
 * @brief
 *
 * @author  Erick Cafferata
 * @date    03.07.2020
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

#include <bsp/stm32cube_hal.h>
#include <config/board_config.h>

#include "stm32cube_udc_priv.h"

EMBOX_UNIT_INIT(stm32cube_udc_init);

#define USB_IRQ_NUM       CONF_USB_OTG_IRQ

#define STM32CUBE_UDC_IN_EP_MASK  ((1 << 1) | (1 << 2) | (1 << 3))
#define STM32CUBE_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))

extern void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);
static irq_return_t stm32cube_usbd_usb_irq_handler(unsigned int irq_nr, void *data) {
	PCD_HandleTypeDef *hpcd = data;
	if (log_level_self() >= LOG_DEBUG) {
		printk("usb: irq entry\n");
	}
	HAL_PCD_IRQHandler(hpcd);
	return IRQ_HANDLED;
}

static int stm32cube_udc_start(struct usb_udc *udc) {

	stm32cube_usbd_init(&stm32cube_udc);

	return 0;
}

static int stm32cube_udc_ep_queue(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {
	struct stm32cube_udc *u = (struct stm32cube_udc *)ep->udc;
	uint8_t n = (uint8_t)ep->nr;
	unsigned int idx;

	assert(ep && req);

	log_debug("EPQ: ep=%u dir=%s len=%u",
	    n, ep->dir == USB_DIR_IN ? "IN" : "OUT", (unsigned)req->len);

	if (n == 0 || ep->dir == USB_DIR_IN) {
		idx = stm32cube_udc_ep_in_idx(n);

		if (u->ep_info[idx].is_used) {
			return -EBUSY;
		}
		u->ep_info[idx].is_used = 1;
		u->ep_info[idx].total_length = req->len;
		u->ep_info[idx].rem_length = req->len;

		u->requests[idx] = req;
		u->eps[idx] = ep;

		HAL_PCD_EP_Transmit(&u->hpcd, n, req->buf, req->len);
		return 0;
	}

	idx = stm32cube_udc_ep_out_idx(n);

	if (u->ep_info[idx].is_used) {
		return -EBUSY;
	}
	u->ep_info[idx].is_used = 1;
	u->ep_info[idx].total_length = req->len;
	u->ep_info[idx].rem_length = req->len;

	u->requests[idx] = req;
	u->eps[idx] = ep;

	HAL_PCD_EP_Receive(&u->hpcd, n, req->buf, req->len);
	return 0;
}

static void stm32cube_udc_ep_enable(struct usb_gadget_ep *ep) {
	struct stm32cube_udc *u;
	unsigned int idx;
	uint8_t ep_addr;
	uint16_t maxpacket;
	uint8_t type;

	assert(ep);
	assert(ep->desc);

	u = (struct stm32cube_udc *)ep->udc;
	ep_addr = ep->desc->b_endpoint_address;
	maxpacket = ep->desc->w_max_packet_size;
	type = usb_gadget_ep_type(ep);

	if (ep->dir == USB_DIR_IN) {
		idx = stm32cube_udc_ep_in_idx(ep->nr);
	}
	else {
		idx = stm32cube_udc_ep_out_idx(ep->nr);
	}

	u->ep_info[idx].maxpacket = maxpacket;

	log_debug("EPO: addr=0x%02x mps=%u type=%u",
	    ep_addr, (unsigned)maxpacket, (unsigned)type);

	HAL_PCD_EP_Open(&u->hpcd, ep_addr, maxpacket, type);
}

struct stm32cube_udc stm32cube_udc = {
    .udc =
        {
            .name = "stm32_udc",
            .udc_start = stm32cube_udc_start,
            .ep_queue = stm32cube_udc_ep_queue,
            .ep_enable = stm32cube_udc_ep_enable,

            .in_ep_mask = STM32CUBE_UDC_IN_EP_MASK,
            .out_ep_mask = STM32CUBE_UDC_OUT_EP_MASK,

			.udc_ep0_max_size = USB_MAX_EP0_SIZE,
			.udc_ep_max_size = CONF_USB_OTG_MISC_EP_MAX_SIZE,
        },
};

static int stm32cube_udc_init(void) {
	int ret;

	ret = irq_attach(USB_IRQ_NUM, stm32cube_usbd_usb_irq_handler, 0, &stm32cube_udc.hpcd, "stm32_udc");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	usb_gadget_udc_register(&stm32cube_udc.udc);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ_NUM, stm32cube_usbd_usb_irq_handler, &stm32cube_udc.hpcd);