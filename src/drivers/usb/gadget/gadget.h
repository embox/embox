/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#ifndef DRIVERS_USB_GADGET_GADGET_H_
#define DRIVERS_USB_GADGET_GADGET_H_

#include <util/dlist.h>
#include <drivers/usb/usb_desc.h>

#define USB_GADGET_STR_MAX          16
#define USB_GADGET_REQ_BUFSIZE      2048
#define USB_GADGET_MAX_INTERFACES   8

struct usb_udc;

struct usb_gadget_ep {
	unsigned char nr;
	unsigned char dir;
	struct usb_udc *udc;
	struct usb_desc_endpoint *desc;
};

struct usb_gadget_request {
	unsigned int len;
	unsigned int actual_len;
	uint8_t buf[USB_GADGET_REQ_BUFSIZE];
	int (*complete)(struct usb_gadget_ep *,
	                    struct usb_gadget_request *);
};

struct usb_gadget {
	struct usb_desc_device device_desc;
	struct usb_desc_configuration config_desc;
	char *strings[USB_GADGET_STR_MAX];

	/* Each interface points to the corresponding USB function. */
	struct usb_gadget_function *interfaces[USB_GADGET_MAX_INTERFACES];
	unsigned int intf_count;

	struct usb_gadget_request req;
	struct usb_gadget_ep ep0;
};

struct usb_gadget_function {
	const struct usb_desc_common_header **descs;
	const char *name;

	struct usb_gadget *gadget;

	struct dlist_head link;

	int (*probe)(struct usb_gadget *);
	int (*enumerate)(struct usb_gadget_function *);
	int (*setup)(struct usb_gadget_function *, const struct usb_control_header *, uint8_t *);
	void (*event)(struct usb_gadget_function *, int event);
};

extern int usb_gadget_setup(struct usb_gadget *gadget,
	const struct usb_control_header *ctrl, uint8_t *buffer);

extern int usb_gadget_enumerate(struct usb_gadget *gadget);

extern int usb_gadget_register(struct usb_gadget *gadget);

extern void usb_gadget_register_function(struct usb_gadget_function *f);

extern int usb_gadget_add_function(struct usb_gadget *gadget,
	                              const char *func_name);

extern int usb_gadget_add_interface(struct usb_gadget *gadget,
	                                struct usb_gadget_function *f);

#endif /* DRIVERS_USB_GADGET_GADGET_H_ */
