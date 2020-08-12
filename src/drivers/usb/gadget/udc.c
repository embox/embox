/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#include <assert.h>
#include <stdint.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

static struct usb_udc *global_udc;

int usb_gadget_udc_start(struct usb_udc *udc) {
	assert(udc);
	assert(udc->udc_start);

	return udc->udc_start(udc);
}

int usb_gadget_ep_queue(struct usb_gadget_ep *ep,
	                    struct usb_gadget_request *req) {
	struct usb_udc *udc = ep->udc;

	assert(udc);
	assert(udc->ep_queue);

	return udc->ep_queue(ep, req);
}

void usb_gadget_ep_enable(struct usb_gadget_ep *ep) {
	struct usb_udc *udc = ep->udc;

	assert(udc);
	assert(udc->ep_enable);

	udc->ep_enable(ep);
}

void usb_gadget_udc_event(struct usb_udc *udc, int event) {
	struct usb_gadget *gadget = udc->composite->config;
	struct usb_gadget_function *func = NULL, *prev_func = NULL;
	int i;

	//assert(gadget);
	if (!gadget) {
		return;
	}

	for (i = 0;; i++) {
		func = gadget->interfaces[i];
		if (!func) {
			break; /* all functions processed */
		}

		if (func == prev_func) {
			continue;
		}

		if (func->event) {
			func->event(func, event);
		}

		prev_func = func;
	}
}

int usb_gadget_register_udc(struct usb_udc *udc) {
	global_udc = udc;
	return 0;
}

struct usb_udc *usb_gadget_find_udc(void) {
	return global_udc;
}
