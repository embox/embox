/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#include <util/log.h>

#include <assert.h>
#include <stdint.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

static struct usb_udc *global_udc;

int usb_gadget_udc_start(struct usb_udc *udc) {
	assert(udc);
	assert(udc->udc_start);

	log_debug("udc start name %s", udc->name);

	return udc->udc_start(udc);
}

int usb_gadget_ep_queue(struct usb_gadget_ep *ep,
	                    struct usb_gadget_request *req) {
	int res;
	struct usb_udc *udc;
	
	udc = ep->udc;

	assert(udc);
	assert(udc->ep_queue);

	log_debug("udc usb_gadget_ep_queue actual_len=%d", req->actual_len);
#if 0
#if __MAX_LOG_LEVEL >= LOG_DEBUG
	__log_handle(LOG_DEBUG, "request actual_len=%d\n", req->actual_len);

	for(int i; i < req->actual_len; i += 8) {
		__log_handle(LOG_DEBUG, "%x:%x:%x:%x:%x:%x:%x:%x\n",
			req->buf[i + 0], req->buf[i + 1], req->buf[i + 2], req->buf[i + 3],
			req->buf[i + 4], req->buf[i + 5], req->buf[i + 6], req->buf[i + 7]);

		}
#endif /* __MAX_LOG_LEVEL >= LOG_DEBUG */
#endif

	res = udc->ep_queue(ep, req);

	return res;
}

void usb_gadget_ep_enable(struct usb_gadget_ep *ep) {
	struct usb_udc *udc = ep->udc;

	assert(udc);
	assert(udc->ep_enable);

	log_debug("uusb_gadget_ep_enable %d", ep->nr);

	udc->ep_enable(ep);
}

void usb_gadget_udc_event(struct usb_udc *udc, int event) {
	struct usb_gadget *gadget = udc->composite->config;
	struct usb_gadget_function *func = NULL, *prev_func = NULL;
	int i;

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
