/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.10.2013
 */

#include <mem/misc/pool.h>
#include <kernel/printk.h>
#include <kernel/panic.h>
#include <embox/unit.h>
#include <drivers/usb/usb_hid.h>
#include <drivers/usb/usb_driver.h>

EMBOX_UNIT_INIT(usb_hid_init);

POOL_DEF(hid_getconfs, struct usb_hid_getconf, USB_HID_MAX_GETCONFS);
POOL_DEF(hid_classes, struct usb_class_hid, USB_HID_MAX_DEVS);

static void usb_class_hid_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_hid *hid = usb2hiddata(dev);

	usb_dev_generic_fill_iface(dev, &hid->getconf->interface_desc);
	usb_dev_generic_fill_endps(dev, hid->getconf->endp_descs);

	pool_free(&hid_getconfs, hid->getconf);
	hid->getconf = NULL;

	usb_class_start_handle(dev);

	usb_driver_handle(req->endp->dev);
}

static void *usb_class_hid_alloc(struct usb_class *cls, struct usb_dev *dev) {
	return pool_alloc(&hid_classes);
}

static void usb_class_hid_free(struct usb_class *cls, struct usb_dev *dev, void *spec) {
	pool_free(&hid_classes, spec);
}

static int usb_class_hid_get_conf(struct usb_class *cls, struct usb_dev *dev) {
	struct usb_class_hid *hid = usb2hiddata(dev);

	hid->getconf = pool_alloc(&hid_getconfs);

	usb_endp_control(dev->endpoints[0], cls->get_conf_hnd, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		USB_DESC_TYPE_CONFIG << 8,
		dev->c_config,
		sizeof(struct usb_desc_configuration)
			+ sizeof(struct usb_desc_interface)
			+ sizeof(struct usb_desc_hid)
			+ (dev->endp_n - 1) * sizeof(struct usb_desc_endpoint),
		hid->getconf);

	return 0;
}

static void usb_class_hid_handle(struct usb_class *cls, struct usb_dev *dev) {
	usb_hid_found(dev);
}

static void usb_class_hid_release(struct usb_class *cls, struct usb_dev *dev) {

	/* can't release, do nothing */
}

static struct usb_class usb_class_hid = {
	.usb_class = USB_CLASS_HID,
	.class_alloc = usb_class_hid_alloc,
	.class_free = usb_class_hid_free,
	.get_conf = usb_class_hid_get_conf,
	.get_conf_hnd = usb_class_hid_get_conf_hnd,
	.class_handle = usb_class_hid_handle,
	.class_release = usb_class_hid_release ,
};

static int usb_hid_init(void) {

	return usb_class_register(&usb_class_hid);
}
