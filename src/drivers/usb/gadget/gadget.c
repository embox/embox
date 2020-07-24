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
#include <util/dlist.h>
#include <util/math.h>
#include <util/log.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

static DLIST_DEFINE(usb_gadget_func_list);

static int usb_gadget_prepare_config_desc(struct usb_gadget *gadget,
	                                uint16_t w_value) {
	int i, j, size = 0;
	uint8_t *buf = gadget->req.buf;
	struct usb_gadget_function *func = NULL, *prev_func = NULL;

	log_debug("conf=%d", w_value);

	/* Copy configuration descriptor */
	memcpy(buf, &gadget->config_desc, gadget->config_desc.b_length);
	buf += gadget->config_desc.b_length;
	size += gadget->config_desc.b_length;

	/* Copy functional descriptors (interfaces) */
	for (i = 0;; i++) {
		func = gadget->interfaces[i];
		if (!func) {
			break; /* all functions processed */
		}

		if (func == prev_func) {
			continue;
		}

		for (j = 0;; j++) {
			if (!func->descs[j]) {
				break; /* all descriptors processed */
			}
			memcpy(buf, func->descs[j], func->descs[j]->b_length);
			buf += func->descs[j]->b_length;
			size += func->descs[j]->b_length;
		}

		prev_func = func;
	}
	((struct usb_desc_configuration *) gadget->req.buf)->w_total_length = size;

	return size;
}

static int usb_gadget_prepare_string_desc(struct usb_gadget *gadget,
	                                uint16_t w_value) {
	const char *str;
	uint8_t *buf = gadget->req.buf;
	int len, wlen, i;
	/* English US */
	uint8_t lang[] = { 0x04, USB_DESC_TYPE_STRING, 0x09, 0x04 };

	log_debug("str=%d", w_value);

	assert(w_value < USB_GADGET_STR_MAX);

	/* Zero value is language ID. */
	if (w_value == 0) {
		memcpy(buf, lang, sizeof lang);
		return sizeof lang;
	}

	str = gadget->strings[w_value];
	len = strlen(str);
	wlen = 2 + (2 * len);

	*buf++ = wlen;
	*buf++ = USB_DESC_TYPE_STRING;

	/* It's UTF-8 to UTF-16 convertion. */
	for (i = 0; i < len; i++) {
		*buf++ = *str++;
		*buf++ = 0;
	}

	return wlen;
}

int usb_gadget_setup(struct usb_gadget *gadget,
		const struct usb_control_header *ctrl, uint8_t *buffer) {
	struct usb_gadget_request *req = &gadget->req;
	struct usb_gadget_function *f = NULL;
	int len, intf;

	if ((ctrl->bm_request_type & USB_REQ_TYPE_MASK) != USB_REQ_TYPE_STANDARD) {
		goto func_setup;
	}

	switch (ctrl->b_request) {
	case USB_REQ_GET_STATUS:
		req->len = sizeof (uint16_t);
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
			goto func_setup;
		}
		break;
	default:
		goto func_setup;
	}

func_setup:

	switch (ctrl->bm_request_type & USB_REQ_RECIP_MASK) {
	case USB_REQ_RECIP_IFACE:
		intf = ctrl->w_index & 0xff;

		if (intf < gadget->intf_count) {
			f = gadget->interfaces[intf];
		}
		break;
	default:
		/* Do nothing, function is NULL */
		break;
	}

	if (f && f->setup) {
		return f->setup(f, ctrl, buffer);
	}

	log_error("Unsupported req: bm_request_type=0x%02x, b_request=0x%02x",
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

void usb_gadget_register_function(struct usb_gadget_function *func) {
	dlist_head_init(&func->link);
	dlist_add_next(&func->link, &usb_gadget_func_list);
}

int usb_gadget_add_function(struct usb_gadget *gadget,
	                         const char *func_name) {
	int res;
	struct usb_gadget_function *func;

	/* Check each hub for event occured. */
	dlist_foreach_entry(func, &usb_gadget_func_list, link) {
		if (!strcmp(func->name, func_name)) {
			assert(func->probe);
			res = func->probe(gadget);
			if (!res) {
				func->gadget = gadget;
			}
			return res;
		}
	}
	return -1; /* func not found */
}

int usb_gadget_add_interface(struct usb_gadget *gadget,
	                         struct usb_gadget_function *func) {
	int id;

	id = gadget->intf_count;
	if (id >= sizeof gadget->interfaces) {
		return -1;
	}
	gadget->interfaces[gadget->intf_count++] = func;

	gadget->config_desc.b_num_interfaces = gadget->intf_count;

	return id;
}

int usb_gadget_enumerate(struct usb_gadget *gadget) {
	struct usb_gadget_function *func = NULL, *prev_func = NULL;
	int i;

	assert(gadget);

	for (i = 0;; i++) {
		func = gadget->interfaces[i];
		if (!func) {
			break; /* all functions processed */
		}

		if (func == prev_func) {
			continue;
		}

		if (func->enumerate) {
			if (func->enumerate(func) != 0) {
				return -1;
			}
		}

		prev_func = func;
	}

	return 0;
}
