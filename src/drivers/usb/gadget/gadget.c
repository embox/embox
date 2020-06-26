/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <util/array.h>
#include <util/math.h>
#include <util/log.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

static int usb_gadget_prepare_config_desc(struct usb_gadget *gadget,
	                                uint16_t w_value) {
	int i, size = 0;
	uint8_t *buf = gadget->req.buf;

	log_debug("conf=%d", w_value);

	for (i = 0;; i++) {
		if (!gadget->descs[i]) {
			break; /* all descriptors processed */
		}
		memcpy(buf, gadget->descs[i], gadget->descs[i]->b_length);
		buf += gadget->descs[i]->b_length;
		size += gadget->descs[i]->b_length;
	}
	((struct usb_desc_configuration *) gadget->req.buf)->w_total_length = size;

	return size;
}

static int usb_gadget_prepare_string_desc(struct usb_gadget *gadget,
	                                uint16_t w_value) {
	const char *str;
	uint8_t *buf = gadget->req.buf;
	int len, wlen, i;

	log_debug("str=%d", w_value);

	assert(w_value < USB_GADGET_STR_MAX);

	str = gadget->strings[w_value];
	len = strlen(str);
	wlen = 2 + (2 * len);

	*buf++ = wlen;
	*buf++ = USB_DESC_TYPE_STRING;

	for (i = 0; i < len; i++) {
		*buf++ = *str++;
		*buf++ = 0;
	}

	return wlen;
}

int usb_gadget_setup(struct usb_gadget *gadget,
	                 const struct usb_control_header *ctrl) {
	struct usb_gadget_request *req = &gadget->req;
	struct usb_function *f = NULL;
	int len;

	log_debug("bm_request_type=0x%02x, b_request=0x%02x",
		ctrl->bm_request_type, ctrl->b_request);

	if ((ctrl->bm_request_type & USB_REQ_TYPE_MASK) != USB_REQ_TYPE_STANDARD) {
		goto intf_setup;
	}

	switch (ctrl->b_request) {
	case USB_REQ_GET_STATUS:
		req->len = 2;
		*(uint16_t *) req->buf = 0;
		goto submit_req;
	case USB_REQ_GET_DESCRIPTOR:
		switch (ctrl->w_value >> 8) {
		case USB_DESC_TYPE_DEV:
			req->len = min(ctrl->w_length, sizeof gadget->device_desc);
			memcpy(req->buf, &gadget->device_desc, req->len);
			goto submit_req;
		case USB_DESC_TYPE_CONFIG:
			len = usb_gadget_prepare_config_desc(gadget,
						ctrl->w_value & 0xff);
			req->len = min(ctrl->w_length, len);
			goto submit_req;
		case USB_DESC_TYPE_STRING:
			len = usb_gadget_prepare_string_desc(gadget,
						ctrl->w_value & 0xff);
			req->len = min(ctrl->w_length, len);
			goto submit_req;
		default:
			goto intf_setup;
		}
		break;
	default:
		goto intf_setup;
	}

intf_setup:
	if (f && f->setup) {
		return f->setup(gadget, ctrl);
	}
	log_debug("Unsupported req: bm_request_type=0x%02x, b_request=0x%02x",
		ctrl->bm_request_type, ctrl->b_request);
	return -1;

submit_req:
	usb_gadget_ep_queue(&gadget->ep0, req);

	return 0;
}

int usb_gadget_register(struct usb_gadget *gadget) {
	struct usb_udc *udc = usb_gadget_find_udc();

	assert(udc);

	gadget->ep0.nr = 0;
	gadget->ep0.udc = udc;

	udc->gadget = gadget;

	return 0;
}
