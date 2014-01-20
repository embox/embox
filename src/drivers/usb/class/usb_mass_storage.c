/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.01.2014
 */

#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <drivers/usb/usb.h>

#include "usb_mass_storage.h"

EMBOX_UNIT_INIT(usb_mass_init);

POOL_DEF(usb_mass_classes, struct usb_mass, USB_MASS_MAX_DEVS);

static void *usb_class_mass_alloc(struct usb_class *cls, struct usb_dev *dev) {
	return pool_alloc(&usb_mass_classes);
}

static void usb_class_mass_free(struct usb_class *cls, struct usb_dev *dev, void *spec) {
	pool_free(&usb_mass_classes, spec);
}

static void usb_mass_reset_hnd(struct usb_request *req, void *arg) {
	usb_scsi_handle(req->endp->dev);
}

static void usb_class_mass_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_mass *mass = usb2massdata(dev);
	int i;

	usb_class_generic_get_conf_hnd(req);

	mass->blkin = mass->blkout = -1;

	for (i = 1; i < dev->endp_n; i++) {
		struct usb_endp *endp = dev->endpoints[i];

		if (endp->type == USB_COMM_BULK) {
			if (endp->direction == USB_DIRECTION_IN) {
				mass->blkin = i;
			}
			if (endp->direction == USB_DIRECTION_OUT) {
				mass->blkout = i;
			}
		}
	}

	usb_endp_control(dev->endpoints[0], usb_mass_reset_hnd,
			USB_DEV_REQ_TYPE_WR | USB_DEV_REQ_TYPE_CLS
				| USB_DEV_REQ_TYPE_IFC,
			USB_REQ_MASS_RESET, 0,
			dev->iface_desc.b_interface_number, 0, NULL);

}

static struct usb_class usb_class_mass = {
	.usb_class = USB_CLASS_MASS,
	.class_alloc = usb_class_mass_alloc,
	.class_free = usb_class_mass_free,
	.get_conf = usb_class_generic_get_conf,
	.get_conf_hnd = usb_class_mass_get_conf_hnd,
};

static int usb_mass_init(void) {

	return usb_class_register(&usb_class_mass);
}
