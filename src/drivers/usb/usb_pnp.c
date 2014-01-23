/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.01.2014
 */

#include <util/dlist.h>
#include <kernel/printk.h>
#include <drivers/usb/usb_driver.h>

#include <drivers/usb/usb.h>

DLIST_DEFINE(usb_dev_list);

static int usb_dev_register(struct usb_dev *dev) {

	dlist_add_next(&dev->dev_link, &usb_dev_list);

	printk("usb_core: vendor=%04x product=%04x is ready\n",
			dev->dev_desc.id_vendor, dev->dev_desc.id_product);
	return 0;
}

static __attribute__((used)) void usb_dev_deregister(struct usb_dev *dev) {

	if (dlist_empty(&dev->dev_link)) {
		return;
	}

	dlist_del(&dev->dev_link);
	dlist_head_init(&dev->dev_link);
}

struct usb_dev *usb_dev_iterate(struct usb_dev *dev) {
	struct dlist_head *dev_link;

	if (dev) {
		dev_link = dev->dev_link.next;
	} else {
		dev_link = usb_dev_list.next;
	}

	if (dev_link != &usb_dev_list) {
		return member_cast_out(dev_link, struct usb_dev, dev_link);
	} else {
		return NULL;
	}
}

void usb_port_detached(struct usb_hub_port *port) {
	assert(0, "%s: cannot handle disconnect", __func__);
}

void usb_port_attached(struct usb_hub_port *port) {
	struct usb_dev *dev;

	assert(port->dev == NULL);

	dev = usb_dev_alloc(port->hub->hcd);
	usb_port_device_bind(port, dev);

	usb_port_reset_lock(port, 0);
}

void usb_port_reset_done(struct usb_hub_port *port) {
	usb_dev_addr_assign(port->dev);
}

void usb_dev_addr_assigned(struct usb_dev *dev) {
	usb_port_reset_unlock(dev->port);
}

void usb_dev_configured(struct usb_dev *dev) {

	usb_dev_register(dev);

	usb_driver_handle(dev);

	usb_class_handle(dev);

}
