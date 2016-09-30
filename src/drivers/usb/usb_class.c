/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.10.2013
 */

#include <errno.h>
#include <string.h>
#include <util/dlist.h>
#include <kernel/printk.h>
#include <kernel/panic.h>
#include <drivers/usb/usb.h>
#include <drivers/usb/usb_driver.h>

static struct usb_class *usb_class_find(struct usb_dev *dev);

static DLIST_DEFINE(usb_classes);

void usb_class_start_handle(struct usb_dev *dev) {
	struct usb_class *cls;

	cls = usb_class_find(dev);
	if (cls->class_handle) {
		cls->class_handle(cls, dev);
	}
}

void usb_class_generic_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;

	usb_dev_generic_fill_iface(dev, &dev->getconf_data->interface_desc);
	usb_dev_generic_fill_endps(dev, dev->getconf_data->endp_descs);

	usb_class_start_handle(dev);

	usb_driver_handle(req->endp->dev);
}

int usb_class_generic_get_conf(struct usb_class *class, struct usb_dev *dev) {
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	usb_endp_control(ctrl_endp, class->get_conf_hnd, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		USB_DESC_TYPE_CONFIG << 8,
		dev->c_config,
		sizeof(struct usb_desc_configuration) + sizeof(struct usb_desc_interface) +
			(dev->endp_n - 1) * sizeof(struct usb_desc_endpoint),
		dev->getconf_data);

	return 0;
}

int usb_dev_generic_fill_iface(struct usb_dev *dev, struct usb_desc_interface *idesc) {

	memcpy(&dev->iface_desc, idesc, sizeof(struct usb_desc_interface));

	return 0;
}

int usb_dev_generic_fill_endps(struct usb_dev *dev, struct usb_desc_endpoint endp_descs[]) {

	for (int i = 1; i < dev->endp_n; i++) {
		struct usb_desc_endpoint *endp_desc = &endp_descs[i - 1];

		assert(endp_desc->b_desc_type == USB_DESC_TYPE_ENDPOINT);

		if (NULL == usb_endp_alloc(dev, endp_desc)) {
			panic("%s: failed to alloc endpoint\n",
					__func__);
		}
	}

	return 0;
}

static void usb_class_fallback_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	int i;

	for (i = 0; i < dev->endp_n - 1; i++) {
		struct usb_desc_endpoint *endp_desc =
			&dev->getconf_data->endp_descs[i];

		if (endp_desc->b_length != sizeof(struct usb_desc_endpoint)
				|| endp_desc->b_desc_type
					!= USB_DESC_TYPE_ENDPOINT) {

			printk("usb_core: vendor=%04x product=%04x "
					"seems cannot be handled by generic "
					"class; disabling\n",
					dev->dev_desc.id_vendor,
					dev->dev_desc.id_product);

#if 0
			usb_dev_deregister(dev);
#endif

			return;
		}
	}

	usb_class_generic_get_conf_hnd(req, NULL);
}

static struct usb_class usb_class_fallback = {
	.get_conf = usb_class_generic_get_conf,
	.get_conf_hnd = usb_class_fallback_get_conf_hnd,
};

int usb_class_register(struct usb_class *cls) {

	assert(cls->get_conf);

	dlist_head_init(&cls->lnk);
	dlist_add_next(&cls->lnk, &usb_classes);

	return 0;
}

static struct usb_class *usb_class_find(struct usb_dev *dev) {
	usb_class_t ucls = usb_dev_class(dev);
	struct usb_class *cls;

	dlist_foreach_entry(cls, &usb_classes, lnk) {
		if (cls->usb_class == ucls) {
			return cls;
		}
	}

	return &usb_class_fallback;
}

int usb_class_supported(struct usb_dev *dev) {

	return &usb_class_fallback != usb_class_find(dev);
}

int usb_class_handle(struct usb_dev *dev) {
	struct usb_class *cls = usb_class_find(dev);

	if (!cls) {
		return -ENOTSUP;
	}

	usb_dev_use_inc(dev);

	if (cls->class_alloc) {
		dev->class_specific = cls->class_alloc(cls, dev);
	}

	return cls->get_conf(cls, dev);
}

void usb_class_release(struct usb_dev *dev) {
	struct usb_class *cls = usb_class_find(dev);

	if (cls->class_release) {
		cls->class_release(cls, dev);
	} else {
		usb_class_released(dev);
	}
}

void usb_class_released(struct usb_dev *dev) {
	struct usb_class *cls = usb_class_find(dev);

	usb_dev_use_dec(dev);

	if (cls->class_free) {
		cls->class_free(cls, dev, dev->class_specific);
	}
}
