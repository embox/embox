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

static void usb_dev_unregister(struct usb_dev *dev) {

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

void usb_port_reset_done(struct usb_hub_port *port) {
	struct usb_dev *dev;

	assert(port->dev == NULL);

	dev = usb_dev_alloc(port->hub->hcd);
	usb_port_device_bind(port, dev);

	dev->plug_state = USB_DEV_DEFAULT;

	usb_dev_addr_assign(dev);
}

void usb_dev_addr_assigned(struct usb_dev *dev) {

	usb_port_reset_unlock(dev->port);

	dev->plug_state = USB_DEV_ADDRESSED;

	dev->bus_idx = dev->idx;

	usb_port_address_setle_wait(dev->port, 10);
}

void usb_dev_addr_settled(struct usb_dev *dev) {
	usb_dev_configure(dev);
}

void usb_dev_configured(struct usb_dev *dev) {

	dev->plug_state = USB_DEV_CONFIGURED;

	usb_dev_register(dev);

	usb_class_handle(dev);

	usb_driver_handle(dev);
}

void usb_dev_disconnect(struct usb_hub_port *port) {
	struct usb_dev *dev = port->dev;
	enum usb_dev_plug_state old_state = dev->plug_state;

	usb_dev_use_inc(dev);

	dev->plug_state = USB_DEV_DETACHED;

	if (old_state == USB_DEV_CONFIGURED) {
		usb_class_release(dev);
		usb_driver_release(dev);
		usb_dev_unregister(dev);
	}

	usb_port_device_unbind(port, dev);
	usb_dev_request_delete(dev);

	usb_dev_use_dec(dev);
}
