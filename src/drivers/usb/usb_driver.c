/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.11.2013
 */

#include <errno.h>
#include <stdio.h>

#include <lib/libds/dlist.h>
// #include <drivers/char_dev.h>
#include <drivers/usb/usb.h>
#include <drivers/usb/usb_dev_desc.h>
#include <fs/inode.h>

#include <drivers/usb/usb_driver.h>

#define USB_DEV_NAME_LEN 14 /* (strlen("usb-0000-0000") + 1) */

static const char *usb_device_name_format = "usb-%04x-%04x";

static DLIST_DEFINE(usb_driver_list);

int usb_driver_register(struct usb_driver *drv) {

	if (drv->probe == NULL) {
		return -EINVAL;
	}

	dlist_head_init(&drv->drv_link);

	dlist_add_next(&drv->drv_link, &usb_driver_list);

	return 0;
}

void *usb_driver_data(struct usb_interface *dev) {
	return dev->driver_data;
}

#if 0
static int usb_driver_id_by_node(struct inode *n, struct usb_device_id *id) {

	if (0 > sscanf(n->name, usb_device_name_format, &id->vid, &id->pid)) {
		return -EINVAL;
	}

	return 0;
}

int usb_driver_open_by_node(struct inode *n, struct usb_dev_desc **ddesc) {
	struct usb_device_id id;

	*ddesc = NULL;

	if (0 != usb_driver_id_by_node(n, &id)) {
		return -EINVAL;
	}

	*ddesc = usb_dev_open(id.vid, id.pid);

	if (!*ddesc) {
		return -ENODEV;
	}

	return 0;
}
#endif

static int usb_driver_match_table(struct usb_interface *dev,
		struct usb_device_id id_table[]) {
	struct usb_device_id *id;

	id = id_table;

	/* Process root hub */
	if (!dev->usb_dev->parent) {
		if (id->dev_class == USB_CLASS_HUB) {
			return 1;
		}
	}

	while (id->dev_class != 0 || id->vid != 0 || id->pid != 0) {
		if ((id->vid == dev->usb_dev->dev_desc.id_vendor &&
				id->pid == dev->usb_dev->dev_desc.id_product) ||
			(id->dev_class == dev->iface_desc[0]->b_interface_class)) {
			return 1;
		}

		id ++;
	}

	return 0;
}

static struct usb_driver *usb_driver_find(struct usb_interface *dev) {
	struct usb_driver *drv;

	dev->driver_data = NULL;
	dev->drv = NULL;

	dlist_foreach_entry(drv, &usb_driver_list, drv_link) {
		int res;

		if (!usb_driver_match_table(dev, drv->id_table)) {
			continue;
		}

		res = drv->probe(dev);
		if (0 != res) {
			continue;
		}

		dev->drv = drv;
		return drv;
	}

	return NULL;
}

int usb_driver_probe(struct usb_interface *dev) {
	struct usb_driver *drv;

	drv = usb_driver_find(dev);

	if (!drv) {
		return -ENOTSUP;
	}

	if (drv->file_ops) {
		char name_buf[USB_DEV_NAME_LEN];

		snprintf(name_buf, USB_DEV_NAME_LEN, usb_device_name_format,
				dev->usb_dev->dev_desc.id_vendor,
				dev->usb_dev->dev_desc.id_product);

		// char_dev_register(NULL);
	}
	return 0;
}

int usb_driver_release(struct usb_interface *dev) {
	struct usb_driver *drv;

	drv = dev->drv;
	if (!drv) {
		return -1;
	}
	drv->disconnect(dev, dev->driver_data);
	return 0;

	/*char_dev_unregister(...)*/

}
