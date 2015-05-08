/**
 * @file
 *
 * @author  Alex Kalmuk
 * @date    05.04.2014
 */

#ifndef USB_CDC_H_
#define USB_CDC_H_

#include <drivers/usb/usb.h>

#define USB_CLASS_CDC       2
#define USB_CDC_MAX_DEVS    4

struct usb_class_cdc {
	/* Storage for selected configuration */
	void *getconf;
	/* Index of current configuration of USB device.
	 * 0 if device is not configured. */
	unsigned int current_conf;
};

static inline struct usb_class_cdc *usb2cdcdata(struct usb_dev *dev) {
	return dev->class_specific;
}

#endif /* USB_CDC_H_ */
