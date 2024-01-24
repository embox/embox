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
#include <lib/libds/array.h>
#include <lib/libds/dlist.h>
#include <util/math.h>
#include <util/log.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

static DLIST_DEFINE(usb_gadget_func_list);

static void usb_gadget_reconfigure(struct usb_gadget *gadget) {
	struct usb_gadget_function *f;
	int i, res;

	gadget->intf_count = 0;
	gadget->out_ep_active_mask = 0;
	gadget->in_ep_active_mask = 0;

	for (i = 0; i < gadget->func_count; i++) {
		f = gadget->functions[i];

		if (f->fini) {
			f->fini(f);
		}

		assert(f->probe);
		res = f->probe(gadget);
		assert(res == 0);
	}
}

static int usb_gadget_prepare_config_desc(
	    struct usb_gadget_composite *composite,
	    uint16_t w_value) {
	int i, j, size = 0;
	uint8_t *buf = composite->req.buf;
	struct usb_gadget *gadget;
	struct usb_gadget_function *func = NULL, *prev_func = NULL;

	log_debug("\nconf=%d\n", w_value);

	gadget = composite->configs[w_value];

	if (composite->config != gadget) {
		usb_gadget_reconfigure(gadget);
		composite->config = gadget;
	}

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
	((struct usb_desc_configuration *) composite->req.buf)->w_total_length = size;

	return size;
}

static int usb_gadget_prepare_string_desc(
	    struct usb_gadget_composite *composite,
	    uint16_t w_value) {
	const char *str;
	uint8_t *buf = composite->req.buf;
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

	str = composite->strings[w_value];
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

int usb_gadget_setup(struct usb_gadget_composite *composite,
		const struct usb_control_header *ctrl, uint8_t *buffer) {
	struct usb_gadget_request *req = &composite->req;
	struct usb_gadget *gadget = composite->config;
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
			req->len = min(ctrl->w_length, sizeof composite->device_desc);
			memcpy(req->buf, &composite->device_desc, req->len);
			goto submit_req;
		case USB_DESC_TYPE_CONFIG:
			len = usb_gadget_prepare_config_desc(composite,
						ctrl->w_value & 0xff);
			req->len = min(ctrl->w_length, len);
			goto submit_req;
		case USB_DESC_TYPE_STRING:
			len = usb_gadget_prepare_string_desc(composite,
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

		assert(gadget);

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
	usb_gadget_ep_queue(&composite->ep0, req);

	return 0;
}

int usb_gadget_register(struct usb_gadget_composite *composite) {
	struct usb_udc *udc = usb_gadget_find_udc();
	struct usb_gadget *gadget;
	int i;

	assert(udc);

	composite->ep0.nr = 0;
	composite->ep0.udc = udc;

	udc->composite = composite;

	for (i = 0; i < USB_GADGET_CONFIGS_MAX; i++) {
		gadget = composite->configs[i];

		if (!gadget) {
			break;
		}

		gadget->composite = composite;
	}

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

			gadget->functions[gadget->func_count++] = func;

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

int usb_gadget_ep_configure(struct usb_gadget *gadget,
	    struct usb_gadget_ep *ep) {
	uint32_t eps_mask, all_eps, active_eps;
	int i;

	if (ep->dir == USB_DIR_OUT) {
		all_eps = ep->udc->out_ep_mask;
		active_eps = gadget->out_ep_active_mask;
	} else {
		all_eps = ep->udc->in_ep_mask;
		active_eps = gadget->in_ep_active_mask;
	}
	eps_mask = all_eps & ~active_eps;

	/* Found next unassigned EP number */
	for (i = 0; i < 8 * sizeof(eps_mask); i++) {
		if ((1 << i) & eps_mask) {
			break;
		}
	}

	if (i == 8 * sizeof(eps_mask)) {
		log_error("Endpoint not allocated");
		return -1;
	}

	ep->nr = i;
	ep->desc->b_endpoint_address = ep->dir | ep->nr;

	if (ep->dir == USB_DIR_OUT) {
		gadget->out_ep_active_mask |= (1 << ep->nr);
	} else {
		gadget->in_ep_active_mask  |= (1 << ep->nr);
	}

	return 0;
}

int usb_gadget_set_config(struct usb_gadget_composite *composite, int config) {
	struct usb_gadget_function *func = NULL, *prev_func = NULL;
	struct usb_gadget *gadget;
	int i;

	composite->config = composite->configs[config - 1];
	gadget = composite->config;

	assert(gadget);

	usb_gadget_reconfigure(gadget);

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
