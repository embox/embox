/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#include <config/custom_config_qspi.h>

#include <hw_cpm.h>
#include <hw_usb.h>

#include <assert.h>
#include <string.h>
#include <util/log.h>
#include <lib/libds/array.h>
#include <util/math.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include "usb_da1469x.h"

EMBOX_UNIT_INIT(da1469x_udc_init);

#define EP0_BUFFER_SIZE 1024
#define DA1469X_UDC_EPS_COUNT 7

#define DA1469x_UDC_IN_EP_MASK   ((1 << 1) | (1 << 3) | (1 << 5))
#define DA1469x_UDC_OUT_EP_MASK  ((1 << 2) | (1 << 4) | (1 << 6))

struct da1469x_udc {
	struct usb_udc udc;
	struct usb_gadget_ep *eps[DA1469X_UDC_EPS_COUNT];
	struct usb_gadget_request *requests[DA1469X_UDC_EPS_COUNT];
	unsigned int setup_buf_pos;
	unsigned int tx_in_progress[DA1469X_UDC_EPS_COUNT];
};

static hw_usb_device_framework_ctrl_req_t usb_setup;

static uint8_t ep0_buffer[EP0_BUFFER_SIZE];

static int da1469x_udc_start(struct usb_udc *udc) {
	usb_da1469x_init();

	return 0;
}

static int da1469x_udc_ep_queue(struct usb_gadget_ep *ep,
	                        struct usb_gadget_request *req) {
	struct da1469x_udc *u = (struct da1469x_udc *) ep->udc;

	assert(ep && req);

	if (ep->nr == 0 || ep->dir == USB_DIR_IN) {
		log_debug("TX, ep=%d, req->len=%d", ep->nr, req->len);

		/* It would be better to use queue here, put req in queue,
		 * then get next req from queue after current finished. */
		while (u->tx_in_progress[ep->nr]) {
		}
		u->tx_in_progress[ep->nr] = 1;

		u->requests[ep->nr] = req;

		hw_usb_ep_tx_start(ep->nr, req->buf, req->len);
	} else {
		log_debug("RX, ep=%d", ep->nr);

		u->requests[ep->nr] = req;
	}

	return 0;
}

static void da1469x_udc_ep_enable(struct usb_gadget_ep *ep) {
	struct da1469x_udc *u = (struct da1469x_udc *) ep->udc;

	assert(ep);

	u->eps[ep->nr] = ep;

	if (0 != ep->nr) {
		/* TODO May be true? */
		hw_usb_ep_configure(ep->nr, false,
		    (hw_usb_device_framework_ep_descriptor_t*) ep->desc);

		if (ep->dir == USB_DIR_OUT) {
			hw_usb_ep_rx_enable(ep->nr);
		}
	}
}

static struct da1469x_udc da1469x_udc = {
	.udc = {
		.name = "da1496x udc",
		.udc_start = da1469x_udc_start,
		.ep_queue = da1469x_udc_ep_queue,
		.ep_enable = da1469x_udc_ep_enable,

		.in_ep_mask = DA1469x_UDC_IN_EP_MASK,
		.out_ep_mask = DA1469x_UDC_OUT_EP_MASK,
	},
};

static void da1469x_udc_send_control_msg(uint8_t *data, uint8_t size) {
	assert(size <= EP0_BUFFER_SIZE);
	
	log_debug("size = %d", size);
	
	memcpy(ep0_buffer, data, size);
	hw_usb_ep_tx_start(0, ep0_buffer, size);
}

void hw_usb_ep_tx_done(uint8_t ep_nr, uint8_t *buffer) {
	struct usb_gadget_request *req;

	da1469x_udc.tx_in_progress[ep_nr] = 0;

	if (ep_nr == 0) {
		hw_usb_ep_rx_enable(0);
		return;
	} else {
		req = da1469x_udc.requests[ep_nr];
		assert(req);

		da1469x_udc.requests[ep_nr] = NULL;

		log_debug("req complete, ep=%d", ep_nr);

		if (req->complete) {
			req->complete(da1469x_udc.eps[ep_nr], req);
		}
	}
}

void hw_usb_bus_event(HW_USB_BUS_EVENT_TYPE event) {
	int i;
	struct usb_gadget_ep *ep;

	switch (event) {
	case UBE_RESET:
		log_debug("Reset event");
		hw_usb_bus_address(0);

		hw_usb_ep_rx_enable(0);

		for (i = 1; i < DA1469X_UDC_EPS_COUNT; i++) {
			ep = da1469x_udc.eps[i];
			if (ep && (ep->dir == USB_DIR_OUT)) {
				hw_usb_ep_rx_enable(ep->nr);
			}
		}
		/* FALLTHROUGH */
	default:
		usb_gadget_udc_event(&da1469x_udc.udc, event);
		break;
	}
}

void hw_usb_bus_frame(uint16_t frame_nr) {
	(void) frame_nr;
}

void hw_usb_ep_nak(uint8_t ep_nr) {
	if (ep_nr == 0) {
		hw_usb_ep_disable(0, false);
		hw_usb_ep_rx_enable(0);
	}
}

void sys_usb_ext_hook_begin_enumeration(void) {
	hw_usb_ep_configure(0, true, NULL);
}

uint8_t *hw_usb_ep_get_rx_buffer(uint8_t ep_nr, bool is_setup, uint16_t *buffer_size) {
	uint8_t *buf;

	if (ep_nr == 0) {
		if (is_setup) {
			da1469x_udc.setup_buf_pos = 0;

			*buffer_size = sizeof(usb_setup);
			return (uint8_t *) &usb_setup;
		} else {
			assert(usb_setup.length <= sizeof ep0_buffer);

			*buffer_size = usb_setup.length - da1469x_udc.setup_buf_pos;
			buf = ep0_buffer + da1469x_udc.setup_buf_pos;
			da1469x_udc.setup_buf_pos += MIN(usb_setup.length - da1469x_udc.setup_buf_pos, 8);

			return buf;
		}
	} else {
		static char rx_ep_buffer[64];

		*buffer_size = 64;

		return (uint8_t *)rx_ep_buffer;
	}

	return NULL;
}

bool hw_usb_ep_rx_read_by_driver(uint8_t ep_nr) {
	return false;
}

static void da1469x_udc_get_status(void) {
	uint16_t status = 0;
	int ep_nr;

	switch (usb_setup.request_type & HW_USB_DEVICE_FRAMEWORK_RECIP_MASK) {
	case HW_USB_DEVICE_FRAMEWORK_RECIP_DEVICE:
		da1469x_udc_send_control_msg((uint8_t *) &status, 2);
		break;
	case HW_USB_DEVICE_FRAMEWORK_RECIP_ENDPOINT:
		ep_nr = usb_setup.index & ~HW_USB_DEVICE_FRAMEWORK_DIR_IN;

#if 0
		if (ep_nr >= RNDIS_EP_COUNT) {
			hw_usb_ep0_stall();
			break;
		}
#endif
		
		if (hw_usb_ep_is_stalled(ep_nr)) {
			status = 1;
		}
		da1469x_udc_send_control_msg((uint8_t *) &status, 2);
		break;
	default:
		log_error("Unsupported RECIP 0x%x",
			usb_setup.request_type & HW_USB_DEVICE_FRAMEWORK_RECIP_MASK);
		hw_usb_ep0_stall();
		break;
	}
}

static void da1469x_udc_set_address(void) {
	log_debug("addr=0x%x", usb_setup.value);
	hw_usb_bus_address(usb_setup.value);
	hw_usb_ep_tx_start(0, NULL, 0);
}

static void da1469x_udc_set_configuration(void) {
	int config = usb_setup.value & 0xff;

	log_debug("\nconf=0x%x\n", config);

	usb_gadget_set_config(da1469x_udc.udc.composite, config);

	hw_usb_ep_tx_start(0, NULL, 0);
}

static void da1469x_udc_handle_standard_req(int ep_nr) {
	int ret;

	switch (usb_setup.request) {
	case HW_USB_DEVICE_FRAMEWORK_REQ_GET_STATUS:
		da1469x_udc_get_status();
		break;
	case HW_USB_DEVICE_FRAMEWORK_REQ_SET_ADDRESS:
		da1469x_udc_set_address();
		break;
	case HW_USB_DEVICE_FRAMEWORK_REQ_GET_CONFIGURATION:
		/* TODO usb_req_get_configuration(); */
		log_debug("GET_CONFIGURATION");
		break;
	case HW_USB_DEVICE_FRAMEWORK_REQ_SET_CONFIGURATION:
		da1469x_udc_set_configuration();
		break;
	default:
		ret = usb_gadget_setup(da1469x_udc.udc.composite,
		         (const struct usb_control_header *) &usb_setup, NULL);
		if (ret != 0) {
			log_error("Not implemented req 0x%x", usb_setup.request);
			hw_usb_ep0_stall();
		}
		break;
	}
}

bool hw_usb_ep_rx_done(uint8_t ep_nr, uint8_t *buffer, uint16_t size) {
	struct usb_gadget_request *req;
	int ret;

	if (ep_nr == 0) {
		if (size == 0) {
			return true;
		}

		if (buffer == (uint8_t *) &usb_setup) {
			if ((usb_setup.request_type & HW_USB_DEVICE_FRAMEWORK_DIR_IN) == 0) {
				if (usb_setup.length) {
					return true; /* Wait for OUT data */
				}
			}
		}

		log_debug("> Usb setup packet:");
		log_debug("request_type=0x%02x, request=0x%02x, value=0x%04x, index=0x%04x, length=0x%04x",
			usb_setup.request_type, usb_setup.request, usb_setup.value, usb_setup.index, usb_setup.length);
		
		switch (usb_setup.request_type & HW_USB_DEVICE_FRAMEWORK_TYPE_MASK) {
		case HW_USB_DEVICE_FRAMEWORK_TYPE_STANDARD:
			da1469x_udc_handle_standard_req(ep_nr);
			break;
		default:
			ret = usb_gadget_setup(da1469x_udc.udc.composite,
			         (const struct usb_control_header *) &usb_setup,
			         ep0_buffer);
			if (ret != 0) {
				log_error("Setup failed, request=0x%x", usb_setup.request);
				hw_usb_ep0_stall();
			}
			break;
		}
	} else {
		req = da1469x_udc.requests[ep_nr];
		assert(req && req->complete);

		da1469x_udc.requests[ep_nr] = NULL;

		req->actual_len = min(size, req->len);
		memcpy(req->buf, buffer, req->actual_len);

		log_debug("req complete, ep=%d, actual_len=%d",
			ep_nr, req->actual_len);

		if (req->complete(da1469x_udc.eps[ep_nr], req) == 0) {
			return true;
		} else {
			return false;
		}
	}

	return false;
}

static int da1469x_udc_init(void) {
	usb_gadget_register_udc(&da1469x_udc.udc);

	return 0;
}
