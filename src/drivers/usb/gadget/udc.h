/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#ifndef DRIVERS_USB_GADGET_UDC_H_
#define DRIVERS_USB_GADGET_UDC_H_

#include <drivers/usb/gadget/gadget.h>

struct usb_udc {
	const char *name;
	int (*ep_queue)(struct usb_gadget_ep *,
	                    struct usb_gadget_request *);

	struct usb_gadget *gadget;
};

extern int usb_gadget_ep_queue(struct usb_gadget_ep *ep,
	struct usb_gadget_request *req);

extern int usb_gadget_register_udc(struct usb_udc *udc);
extern struct usb_udc *usb_gadget_find_udc(void);

#endif /* DRIVERS_USB_GADGET_UDC_H_ */
