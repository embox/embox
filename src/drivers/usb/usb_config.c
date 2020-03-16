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

static struct usb_endp *usb_endp_alloc(struct usb_dev *dev, unsigned n,
		const struct usb_desc_endpoint *ep_desc) {
	struct usb_endp *ep;
	struct usb_hcd *hcd = dev->hcd;

	assert(n < USB_DEV_MAX_ENDP);
	assert(!dev->endpoints[n]);

	ep = pool_alloc(&usb_endps);
	if (!ep) {
		return NULL;
	}
	ep->dev = dev;

	usb_endp_fill_from_desc(ep, ep_desc);

	dev->endpoints[n] = ep;

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

#if 0
static void usb_endp_free(struct usb_endp *endp) {
	struct usb_hcd *hcd = endp->dev->hcd;

	if (hcd->ops->endp_hci_free) {
		hcd->ops->endp_hci_free(endp, endp->hci_specific);
	}

	pool_free(&usb_endps, endp);
}
#endif

static void usb_dev_fill_config(struct usb_dev *dev, void *cur) {
	int i;
	struct usb_desc_common_header *other_desc;
	struct usb_desc_endpoint *endp_desc;

	/* Fill interface. */
	cur += sizeof (struct usb_desc_configuration);
	memcpy(&dev->iface_desc, cur, sizeof(struct usb_desc_interface));

	/* b_num_endpoints excludes control endpoint, so add it. */
	dev->endp_n = dev->iface_desc.b_num_endpoints + 1;
	cur += sizeof (struct usb_desc_interface);

	/* Skip other descriptors till endpoints. */
	other_desc = (struct usb_desc_common_header *) cur;
	while (other_desc->b_desc_type != USB_DESC_TYPE_ENDPOINT) {
		cur += other_desc->b_length;
		other_desc = (struct usb_desc_common_header *) cur;
	}

	/* Fill endpoints. */
	for (i = 1; i < dev->endp_n; i++) {
		endp_desc = (struct usb_desc_endpoint *) cur;
		assert(endp_desc->b_desc_type == USB_DESC_TYPE_ENDPOINT);

		if (NULL == usb_endp_alloc(dev, i, endp_desc)) {
			panic("%s: failed to alloc endpoint\n",
					__func__);
		}

		cur += sizeof (struct usb_desc_endpoint);
	}
}

/* Get full device configuration including interfaces and endpoints. */
int usb_get_configuration(struct usb_dev *dev, unsigned int n) {
	int len, ret;
	struct usb_desc_configuration conf;

	/* Check the configuration is not allocated yet. */
	assert(!dev->config_buf);

	log_debug("dev(%d:%d) conf=%d", dev->bus_idx, dev->addr, n);

	ret = usb_endp_control_wait(dev->endpoints[0],
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
	dev->config_buf = sysmalloc(len);
	if (!dev->config_buf) {
		goto err;
	}

	ret = usb_endp_control_wait(dev->endpoints[0],
		USB_DIR_IN | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_GET_DESCRIPTOR,
		(USB_DESC_TYPE_CONFIG << 8) + n,
		0, len, dev->config_buf, 1000);
	if (ret < 0) {
		goto err;
	}

	usb_dev_fill_config(dev, dev->config_buf);

	log_debug("b_interface_class = 0x%x OK ", dev->iface_desc.b_interface_class);

	return 0;
err:
	if (dev->config_buf) {
		sysfree(dev->config_buf);
	}
	log_error("failed");
	return -1;
}

int usb_set_configuration(struct usb_dev *dev, unsigned int n) {
	int ret;

	log_debug("dev(%d:%d) conf=%d", dev->bus_idx, dev->addr, n);

	ret = usb_endp_control_wait(dev->endpoints[0],
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

int usb_get_ep0(struct usb_dev *dev) {
	if (!usb_endp_alloc(dev, 0, &usb_desc_endp_control_default)) {
		log_error("failed");
		return -1;
	}
	return 0;
}
