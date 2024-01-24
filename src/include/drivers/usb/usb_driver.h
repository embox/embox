/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.11.2013
 */

#ifndef DRIVERS_USB_DRIVER_H_
#define DRIVERS_USB_DRIVER_H_

#include <lib/libds/dlist.h>

struct usb_dev;
struct usb_hcd;
struct usb_driver;
struct file_operations;
struct inode;
struct usb_interface;

struct usb_device_id {
	unsigned short dev_class;
	unsigned short vid;
	unsigned short pid;
};

struct usb_driver {
	const char *name;
	int (*probe)(struct usb_interface *dev);
	void (*disconnect)(struct usb_interface *dev, void *data);
	const struct file_operations *file_ops;
	struct dlist_head drv_link;

	struct usb_device_id *id_table;
};

extern int usb_driver_register(struct usb_driver *drv);
extern int usb_driver_probe(struct usb_interface *dev);
extern int usb_driver_release(struct usb_interface *dev);

#endif /* DRIVERS_USB_DRIVER_H_ */
