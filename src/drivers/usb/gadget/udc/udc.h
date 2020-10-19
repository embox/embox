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

	int (*udc_start)(struct usb_udc *);
	int (*ep_queue)(struct usb_gadget_ep *,
	                    struct usb_gadget_request *);
	void (*ep_enable)(struct usb_gadget_ep *);

	struct usb_gadget_composite *composite;

	/* all available endpoints */
	uint32_t in_ep_mask;
	uint32_t out_ep_mask;
};

extern int usb_gadget_ep_queue(struct usb_gadget_ep *ep,
	struct usb_gadget_request *req);

extern int usb_gadget_register_udc(struct usb_udc *udc);
extern struct usb_udc *usb_gadget_find_udc(void);
extern void usb_gadget_ep_enable(struct usb_gadget_ep *ep);
extern void usb_gadget_udc_event(struct usb_udc *udc, int event);
extern int usb_gadget_udc_start(struct usb_udc *udc);

#endif /* DRIVERS_USB_GADGET_UDC_H_ */
