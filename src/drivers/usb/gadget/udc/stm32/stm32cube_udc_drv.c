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

#include <kernel/irq.h>
#include <lib/libds/array.h>

#include <embox/unit.h>

#include <util/math.h>

#include <bsp/stm32cube_hal.h>

#include "stm32cube_udc_priv.h"

EMBOX_UNIT_INIT(stm32cube_udc_init);

#define STM32CUBE_UDC_IN_EP_MASK  ((1 << 1) | (1 << 2) | (1 << 3))
#define STM32CUBE_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))


static int stm32cube_udc_start(struct usb_udc *udc) {
	stm32cube_usbd_init();

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

		HAL_PCD_EP_Transmit(&hpcd, n, req->buf, req->len);
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

	HAL_PCD_EP_Receive(&hpcd, n, req->buf, req->len);
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
	type = stm32cube_udc_ep_type(ep);

	if (ep->dir == USB_DIR_IN) {
		idx = stm32cube_udc_ep_in_idx(ep->nr);
	}
	else {
		idx = stm32cube_udc_ep_out_idx(ep->nr);
	}

	u->ep_info[idx].maxpacket = maxpacket;

	log_debug("EPO: addr=0x%02x mps=%u type=%u",
	    ep_addr, (unsigned)maxpacket, (unsigned)type);

	HAL_PCD_EP_Open(&hpcd, ep_addr, maxpacket, type);
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
        },
};

static int stm32cube_udc_init(void) {
	usb_gadget_register_udc(&stm32cube_udc.udc);

	return 0;
}
