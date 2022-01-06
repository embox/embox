/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.10.2013
 */

#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>

#include <util/log.h>
#include <kernel/panic.h>
#include <drivers/usb/usb.h>

POOL_DEF(usb_endps, struct usb_endp, USB_MAX_ENDP);

POOL_DEF(usb_ifaces, struct usb_interface, USB_DEV_MAX_INTERFACES * USB_MAX_DEV);

static const struct usb_desc_endpoint usb_desc_endp_control_default = {
	.b_length = sizeof(struct usb_desc_endpoint),
	.b_desc_type = USB_DESC_TYPE_ENDPOINT,
	.b_endpoint_address = 0,
	.bm_attributes = 0,
	.w_max_packet_size = 64,
	.b_interval = 0,
};

static void usb_endp_fill_from_desc(struct usb_endp *endp,
		const struct usb_desc_endpoint *desc) {
	endp->address = desc->b_endpoint_address & USB_DESC_ENDP_ADDR_MASK;

	switch (desc->b_endpoint_address & USB_DESC_ENDP_ADDR_DIR_MASK) {
	case USB_DESC_ENDP_ADDR_OUT:
		endp->direction = USB_DIRECTION_OUT;
		break;
	default:
	case USB_DESC_ENDP_ADDR_IN:
		endp->direction = USB_DIRECTION_IN;
		break;
	}

	switch (desc->bm_attributes & USB_DESC_ENDP_TYPE_MASK) {
	case USB_DESC_ENDP_TYPE_ISOCHR:
		endp->type = USB_COMM_ISOCHRON;
		break;
	case USB_DESC_ENDP_TYPE_BULK:
		endp->type = USB_COMM_BULK;
		break;
	case USB_DESC_ENDP_TYPE_INTR:
		endp->type = USB_COMM_INTERRUPT;
		break;
	default:
	case USB_DESC_ENDP_TYPE_CTRL:
		endp->type = USB_COMM_CONTROL;
		endp->direction = USB_DIRECTION_BI;
		break;
	}
	endp->max_packet_size = desc->w_max_packet_size;
	endp->interval = desc->b_interval;
}

static struct usb_endp *usb_endp_alloc(struct usb_interface *iface, unsigned n,
		const struct usb_desc_endpoint *ep_desc) {
	struct usb_endp *ep;
	struct usb_dev *dev = iface->usb_dev;
	struct usb_hcd *hcd = dev->hcd;

	assert(n < USB_DEV_MAX_ENDP);
	assert(!iface->endpoints[n]);

	ep = pool_alloc(&usb_endps);
	if (!ep) {
		return NULL;
	}
	ep->dev = dev;

	usb_endp_fill_from_desc(ep, ep_desc);

	iface->endpoints[n] = ep;

	if (hcd->ops->endp_hci_alloc) {
		ep->hci_specific = hcd->ops->endp_hci_alloc(ep);
		if (!ep->hci_specific) {
			pool_free(&usb_endps, ep);
			return NULL;
		}
	}

	log_debug("dev(%d:%d) enp(%d) dir(%d) maxpack(%d)", dev->bus_idx, dev->addr, n, ep->direction,ep->max_packet_size);
	return ep;
}

static void usb_endp_free(struct usb_endp *endp) {
	struct usb_hcd *hcd = endp->dev->hcd;

	if (hcd->ops->endp_hci_free) {
		hcd->ops->endp_hci_free(endp, endp->hci_specific);
	}

	pool_free(&usb_endps, endp);
}

int usb_create_root_interface(struct usb_dev *dev) {
	struct usb_interface *iface;

	iface = pool_alloc(&usb_ifaces);
	if (iface == NULL) {
		return -1;
	}
	memset(iface, 0, sizeof(struct usb_interface));

	dev->usb_iface[0] = iface;
	iface->usb_dev = dev;

	return 0;
}

static int usb_create_interface(struct usb_dev *dev, struct usb_desc_interface *iface_desc, int iface_num) {
	struct usb_interface *iface;
	void *cur = iface_desc;
	int j;
	int len = 0;
	struct usb_desc_common_header *other_desc;
	struct usb_desc_endpoint *endp_desc;

	iface = pool_alloc(&usb_ifaces);
	if (iface == NULL) {
		return -1;
	}

	memset(iface, 0, sizeof(struct usb_interface));

	dev->usb_iface[iface_num] = iface;
	iface->usb_dev = dev;
	iface->iface_desc[0] = iface_desc;

	iface_desc = cur;

	len += sizeof(struct usb_desc_interface);
	cur += len;

		/* Skip other descriptors till endpoints. */
	if (iface_desc->b_num_endpoints > 0) {
		other_desc = (struct usb_desc_common_header *) cur;
		while (other_desc->b_desc_type != USB_DESC_TYPE_ENDPOINT) {
			len += other_desc->b_length;
			cur += other_desc->b_length;
			other_desc = (struct usb_desc_common_header *) cur;
		}
	}

	/* Fill endpoints. */
	for (j = 1; j <= iface_desc->b_num_endpoints; j++) {
		endp_desc = (struct usb_desc_endpoint *) cur;
		assert(endp_desc->b_desc_type == USB_DESC_TYPE_ENDPOINT);

		if (NULL == usb_endp_alloc(iface, j, endp_desc)) {
			panic("%s: failed to alloc endpoint\n", 	__func__);

		}
		len += sizeof (struct usb_desc_endpoint);
		cur += sizeof (struct usb_desc_endpoint);
	}
	iface->endp_n += iface_desc->b_num_endpoints;
	log_info("dev (%p) iface[%d] len(%d) endp_n(%d)", dev, iface_num, len, iface->endp_n);

	return len;
}

static void usb_dev_fill_config(struct usb_dev *dev) {
	struct usb_desc_configuration *conf;
	void *cur = dev->current_config->config_buf, *end;
	int i = 0;

	conf = (struct usb_desc_configuration *) cur;
	end = cur + conf->w_total_length;

	cur += sizeof (struct usb_desc_configuration);

	while (cur < end) {
		struct usb_desc_interface *iface_desc;

		/* Fill interface. */
		iface_desc = (struct usb_desc_interface *) cur;
		if (iface_desc->b_desc_type == USB_DESC_TYPE_INTERFACE_ASSOC) {
			//struct usb_desc_interface_assoc *assoc = cur;
			//log_debug("interface_assoc");
			cur += sizeof (struct usb_desc_interface_assoc);
			continue;
		}

		cur += usb_create_interface(dev, iface_desc, i);
		i ++;
	}
	dev->usb_iface_num = i;
}

/* Get full device configuration including interfaces and endpoints. */
int usb_get_configuration(struct usb_dev *dev, unsigned int n) {
	int len, ret;
	struct usb_desc_configuration conf;

	/* Check the configuration is not allocated yet. */
	assert(!dev->current_config->config_buf);

	log_debug("dev(%d:%d) conf=%d", dev->bus_idx, dev->addr, n);

	ret = usb_endp_control_wait(&dev->endp0,
		USB_DIR_IN | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_GET_DESCRIPTOR,
		(USB_DESC_TYPE_CONFIG << 8) + n,
		0, sizeof(struct usb_desc_configuration), &conf, 1000);
	if (ret) {
		goto err;
	}

	log_info("dev(%d:%d) Configuration %d:"
			"\n\t\t  b_length=%d"
			"\n\t\t  b_desc_type=0x%x"
			"\n\t\t  w_total_length=%d"
			"\n\t\t  b_num_interfaces=%d",
			dev->bus_idx, dev->addr, n,
			conf.b_length, conf.b_desc_type, conf.w_total_length, conf.b_num_interfaces);

	/* Now get full configuration. */
	len = conf.w_total_length;
	dev->current_config->config_buf = sysmalloc(len);
	if (!dev->current_config->config_buf) {
		goto err;
	}

	ret = usb_endp_control_wait(&dev->endp0,
		USB_DIR_IN | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_GET_DESCRIPTOR,
		(USB_DESC_TYPE_CONFIG << 8) + n,
		0, len, dev->current_config->config_buf, 1000);
	if (ret < 0) {
		goto err;
	}

	usb_dev_fill_config(dev);

	log_debug("b_interface_class = 0x%x OK ", dev->usb_iface[0]->iface_desc[0]->b_interface_class);

	return 0;
err:
	if (dev->current_config->config_buf) {
		sysfree(dev->current_config->config_buf);
	}
	log_error("failed");
	return -1;
}

int usb_set_configuration(struct usb_dev *dev, unsigned int n) {
	int ret;

	log_debug("dev(%d:%d) conf=%d", dev->bus_idx, dev->addr, n);

	ret = usb_endp_control_wait(&dev->endp0,
		USB_DIR_OUT | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_SET_CONFIG, n,
		0, 0, NULL, 1000);
	if (ret < 0) {
		log_error("failed");
		return -1;
	}
	usleep(1000 * 1000);
	log_debug("ok");
	return 0;
}

void usb_free_configuration(struct usb_dev *dev) {
	int i;

	/* Free endpoints expect endpoint 0. */
	for (i = 1; i < dev->usb_iface[0]->endp_n; i++) {
		usb_endp_free(dev->usb_iface[0]->endpoints[i]);
		dev->usb_iface[0]->endpoints[i] = NULL;
	}

	/* Zero interfaces pointers */
	memset(&dev->usb_iface[0]->iface_desc[0], 0, sizeof(struct usb_desc_interface));

	/* Free configuration */
	sysfree(dev->current_config->config_buf);
	dev->current_config->config_buf = NULL;
}

int usb_get_ep0(struct usb_dev *dev) {
	struct usb_endp *ep;
	struct usb_hcd *hcd = dev->hcd;

	ep = &dev->endp0;

	ep->dev = dev;

	usb_endp_fill_from_desc(ep, &usb_desc_endp_control_default);

	if (hcd->ops->endp_hci_alloc) {
		ep->hci_specific = hcd->ops->endp_hci_alloc(ep);
		if (!ep->hci_specific) {
			log_error("failed");
			return -1;
		}
	}

	return 0;
}

int usb_set_iface(struct usb_dev *dev, int iface, int alt) {
	return usb_endp_control_wait(&dev->endp0,
	            USB_DIR_OUT | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_IFACE,
	            USB_REQ_SET_INTERFACE,
	            iface, alt, 0, NULL, 1000);
}
