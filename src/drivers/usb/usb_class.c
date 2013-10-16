/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.10.2013
 */

#include <errno.h>
#include <kernel/printk.h>
#include <drivers/usb.h>

#define USB_CLASS_HID 		 3

#define USB_CLASS_HID_SUB	 0
#define USB_CLASS_HID_SUB_BOOT	 1

#define USB_CLASS_HID_PROT_NONE  0
#define USB_CLASS_HID_PROT_KBD   1
#define USB_CLASS_HID_PROT_MOUSE 2

#define notzero_or(x, y) \
	(x) ? (x) : (y)

#define INTR_DATA_LEN 4
static char intr_data[INTR_DATA_LEN];

static void usb_class_mouse_notify(struct usb_request *req) {
	printk("\n\nusb_mouse: received=%02x %02x %02x %02x\n",
			intr_data[0], intr_data[1], intr_data[2],
			intr_data[3]);

	usb_endp_interrupt(req->endp->dev->endpoints[1],
			usb_class_mouse_notify,
			intr_data, INTR_DATA_LEN);
}

static int usb_class_handle_mouse(struct usb_dev *dev) {

	usb_endp_interrupt(dev->endpoints[1], usb_class_mouse_notify,
			intr_data, INTR_DATA_LEN);

	return 0;
}

int usb_class_handle(struct usb_dev *dev) {
	struct usb_desc_device *dev_desc = usb_dev_get_desc(dev);
	unsigned char class, subclass, proto;

	class = notzero_or(dev_desc->b_dev_class, dev->interface_desc->b_interface_class);
	subclass = notzero_or(dev_desc->b_dev_subclass, dev->interface_desc->b_interface_subclass);
	proto = notzero_or(dev_desc->b_dev_protocol, dev->interface_desc->b_interface_protocol);

	if ((class != USB_CLASS_HID) || (subclass != USB_CLASS_HID_SUB_BOOT)
				|| (proto != USB_CLASS_HID_PROT_MOUSE)) {
		assert(0);
		return -ENOTSUP;
	}

	return usb_class_handle_mouse(dev);
}

