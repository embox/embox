/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2013
 */

#ifndef DRIVERS_USB_HID_H_
#define DRIVERS_USB_HID_H_

#define USB_HID_MAX_DEVS         2

#define USB_CLASS_HID 		 3

#define USB_CLASS_HID_SUB	 0
#define USB_CLASS_HID_SUB_BOOT	 1

#define USB_CLASS_HID_PROT_NONE  0
#define USB_CLASS_HID_PROT_KBD   1
#define USB_CLASS_HID_PROT_MOUSE 2

#include <drivers/usb/usb.h>

struct input_dev;

struct usb_desc_hid {
	uint8_t b_length;
	uint8_t b_desc_type;
	uint16_t bcd_hid;
	uint8_t b_country_code;
	uint8_t b_num_descriptors;
	uint8_t b_descriptor_type;
	uint16_t w_descriptor_len;
} __attribute__((packed));

struct usb_class_hid {
	struct input_dev *indev;
};

static inline struct usb_class_hid *usb2hiddata(struct usb_dev *dev) {
	return dev->driver_data;
}

extern int usb_hid_found(struct usb_dev *dev);

#endif /* DRIVERS_USB_HID_H_ */

