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
	/* This field can be used by an external driver */
	void *privdata;
};

static inline struct usb_class_cdc *usb2cdcdata(struct usb_dev *dev) {
	return dev->class_specific;
}

/**
 * Set interface with the specified alternated setting
 * @c alternated_cfg, and on success calls callback @c cb
 */
extern void cdc_set_interface(struct usb_dev *dev, size_t iface,
        size_t alternated_cfg, usb_request_notify_hnd_t cb);

/**
 * Obtain the interface, specified by @c index,
 * in the current configuration
 */
extern struct usb_desc_interface *cdc_get_interface(
        struct usb_desc_configuration *conf, size_t index);

#endif /* USB_CDC_H_ */
