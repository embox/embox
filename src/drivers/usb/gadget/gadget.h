/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#ifndef DRIVERS_USB_GADGET_GADGET_H_
#define DRIVERS_USB_GADGET_GADGET_H_

#include <drivers/usb/usb_desc.h>

#define USB_GADGET_STR_MAX      16
#define USB_GADGET_REQ_BUFSIZE  1024

struct usb_udc;

struct usb_gadget_ep {
	unsigned char nr;
	struct usb_udc *udc;
};

struct usb_gadget_request {
	unsigned int len;
	uint8_t buf[USB_GADGET_REQ_BUFSIZE];
};

struct usb_gadget {
	struct usb_desc_device device_desc;
	const struct usb_desc_common_header **descs;
	const char *strings[USB_GADGET_STR_MAX];

	struct usb_gadget_request req;
	struct usb_gadget_ep ep0;
};

struct usb_function {
	int (*setup)(struct usb_gadget *, const struct usb_control_header *);
};

extern int usb_gadget_setup(struct usb_gadget *gadget,
	                 const struct usb_control_header *ctrl);

extern int usb_gadget_register(struct usb_gadget *gadget);

#endif /* DRIVERS_USB_GADGET_GADGET_H_ */
