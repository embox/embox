/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.11.2013
 */

#ifndef DRIVERS_USB_DRIVER_H_
#define DRIVERS_USB_DRIVER_H_

#include <util/dlist.h>

struct usb_dev;
struct usb_driver;
struct file_operations;
struct usb_dev_desc;
struct node;

struct usb_device_id {
	unsigned short vid;
	unsigned short pid;
};

struct usb_driver {
	int (*probe)(struct usb_driver *drv, struct usb_dev *dev, void **data);
	void (*disconnect)(struct usb_dev *dev, void *data);
	const struct file_operations *file_ops;
	struct dlist_head drv_link;

	struct usb_device_id *id_table;
};

extern void *usb_driver_data(struct usb_dev *dev);
extern int usb_driver_open_by_node(struct node *n, struct usb_dev_desc **ddesc);

extern int usb_driver_register(struct usb_driver *drv);

extern void usb_driver_handle(struct usb_dev *dev);
extern void usb_driver_release(struct usb_dev *dev);

#endif /* DRIVERS_USB_DRIVER_H_ */
