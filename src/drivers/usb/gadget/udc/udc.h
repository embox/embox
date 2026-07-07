/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#ifndef DRIVERS_USB_GADGET_UDC_H_
#define DRIVERS_USB_GADGET_UDC_H_

#include <stdint.h>

#include <drivers/usb/usb_desc.h>

struct usb_udc;
struct usb_gadget_ep;
struct usb_gadget_request;
struct usb_gadget_composite;

struct usb_udc_ops {
	int  (*uuo_udc_start)(struct usb_udc *);
	int  (*uuo_ep_queue)(struct usb_gadget_ep *, struct usb_gadget_request *);
	int  (*uuo_ep_stall)(struct usb_gadget_ep *, const struct usb_control_header *);
	void (*uuo_ep_enable)(struct usb_gadget_ep *);
};

struct usb_udc {
	const char *udc_name;

	struct usb_udc_ops *udc_ops;

	struct usb_gadget_composite *udc_composite;

	/* all available endpoints */
	uint32_t udc_in_ep_mask;
	uint32_t udc_out_ep_mask;

	uint8_t  udc_ep0_max_size;
	int      udc_ep_max_size;
};

extern int usb_gadget_ep_queue(struct usb_gadget_ep *ep,
    struct usb_gadget_request *req);

extern int usb_gadget_ep_stall(struct usb_gadget_ep *ep,
    const struct usb_control_header *ctrl);

extern int usb_gadget_udc_register(struct usb_udc *udc);
extern struct usb_udc *usb_gadget_find_udc(void);
extern void usb_gadget_ep_enable(struct usb_gadget_ep *ep);

extern int usb_gadget_udc_start(struct usb_udc *udc);

extern void usb_gadget_udc_event(struct usb_udc *udc, int event);

#endif /* DRIVERS_USB_GADGET_UDC_H_ */
