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

int usb_gadget_ep_queue(struct usb_gadget_ep *ep,
	                    struct usb_gadget_request *req) {
	struct usb_udc *udc = ep->udc;

	assert(udc);
	assert(udc->ep_queue);

	return udc->ep_queue(ep, req);
}

int usb_gadget_register_udc(struct usb_udc *udc) {
	global_udc = udc;
	return 0;
}

struct usb_udc *usb_gadget_find_udc(void) {
	return global_udc;
}
