/*
 * usb_cdc.h
 *
 *  Created on: 2014 4 5
 *      Author: alexander
 */

#ifndef USB_CDC_H_
#define USB_CDC_H_

#include <drivers/usb/usb.h>

#define USB_CLASS_CDC       2
#define USB_CDC_MAX_DEVS    4

struct input_dev;

struct usb_class_cdc {
	void *getconf;
	struct input_dev *indev;
};

static inline struct usb_class_cdc *usb2cdcdata(struct usb_dev *dev) {
	return dev->class_specific;
}

#endif /* USB_CDC_H_ */
