/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#ifndef DRIVERS_USB_GADGET_GADGET_H_
#define DRIVERS_USB_GADGET_GADGET_H_

#include <lib/libds/dlist.h>
#include <drivers/usb/usb_desc.h>

#define USB_GADGET_CONFIGS_MAX      2
#define USB_GADGET_STR_MAX          16
#define USB_GADGET_REQ_BUFSIZE      2048
#define USB_GADGET_MAX_INTERFACES   8
#define USB_GADGET_MAX_FUNCTIONS    4

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
	struct usb_gadget_composite *composite;

	struct usb_desc_configuration config_desc;

	struct usb_gadget_function *functions[USB_GADGET_MAX_FUNCTIONS];
	unsigned int func_count;

	/* Each interface points to the corresponding USB function. */
	struct usb_gadget_function *interfaces[USB_GADGET_MAX_INTERFACES];
	unsigned int intf_count;

	/* Enpdoints reserved for the current gadget (configuration) */
	int out_ep_active_mask;
	int in_ep_active_mask;
};

struct usb_gadget_composite {
	struct usb_desc_device device_desc;
	char *strings[USB_GADGET_STR_MAX];

	struct usb_gadget_request req;
	struct usb_gadget_ep ep0;

	struct usb_gadget *config; /* Current configuration */

	struct usb_gadget *configs[USB_GADGET_CONFIGS_MAX]; /* All configurations */
};

struct usb_gadget_function {
	const char *name;

	const struct usb_desc_common_header **descs;

	struct usb_gadget *gadget;

	struct dlist_head link;

	int (*probe)(struct usb_gadget *);
	int (*fini)(struct usb_gadget_function *);
	int (*enumerate)(struct usb_gadget_function *);
	int (*setup)(struct usb_gadget_function *, const struct usb_control_header *, uint8_t *);
	void (*event)(struct usb_gadget_function *, int event);
};

extern int usb_gadget_setup(struct usb_gadget_composite *composite,
	const struct usb_control_header *ctrl, uint8_t *buffer);

extern int usb_gadget_set_config(struct usb_gadget_composite *composite,
	int config);

extern int usb_gadget_register(struct usb_gadget_composite *gadget);

extern void usb_gadget_register_function(struct usb_gadget_function *f);

extern int usb_gadget_add_function(struct usb_gadget *gadget,
	                              const char *func_name);

extern int usb_gadget_add_interface(struct usb_gadget *gadget,
	                                struct usb_gadget_function *f);

extern int usb_gadget_ep_configure(struct usb_gadget *gadget,
	struct usb_gadget_ep *ep);

#endif /* DRIVERS_USB_GADGET_GADGET_H_ */
