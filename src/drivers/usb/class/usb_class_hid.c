/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.10.2013
 */

#include <embox/unit.h>
#include <mem/misc/pool.h>
#include <drivers/usb/usb_hid.h>
#include <drivers/usb/usb_driver.h>

EMBOX_UNIT_INIT(usb_hid_init);

POOL_DEF(hid_classes, struct usb_class_hid, USB_HID_MAX_DEVS);

static int usb_hid_probe(struct usb_dev *dev) {
	struct usb_class_hid *hid;

	assert(dev);

	hid = pool_alloc(&hid_classes);
	if (!hid) {
		return -1;
	}
	dev->driver_data = hid;

	return usb_hid_found(dev);
}

static void usb_hid_disconnect(struct usb_dev *dev, void *data) {
	/* TODO */
}

static struct usb_device_id usb_hid_id_table[] = {
	{USB_CLASS_HID, 0xffff, 0xffff},
	{ },
};

static struct usb_driver usb_driver_hid = {
	.name = "hid",
	.probe = usb_hid_probe,
	.disconnect = usb_hid_disconnect,
	.id_table = usb_hid_id_table,
};

static int usb_hid_init(void) {
	return usb_driver_register(&usb_driver_hid);
}
