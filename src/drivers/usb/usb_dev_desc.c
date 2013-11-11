/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.11.2013
 */

#include <errno.h>
#include <string.h>
#include <drivers/usb.h>
#include <drivers/usb_desc.h>
#include <mem/misc/pool.h>
#include <kernel/manual_event.h>

#include <drivers/usb_dev_desc.h>

struct usb_dev_desc {
	struct usb_dev *dev;
};

POOL_DEF(usb_dev_descs, struct usb_dev_desc, 2);

struct usb_endp_desc {
	struct usb_endp *endp;
};

POOL_DEF(usb_endp_descs, struct usb_endp_desc, 2);

struct usb_dev_desc *usb_dev_open(uint16_t vid, uint16_t pid) {
	struct usb_dev_desc *ddesc;
	struct usb_dev *dev = usb_dev_iterate(NULL);

	if (!dev) {
		return NULL;
	}

	while (dev->dev_desc.id_vendor != vid && dev->dev_desc.id_product) {

		dev = usb_dev_iterate(dev);
		if (dev == NULL) {
			break;
		}
	}

	if (!dev) {
		return NULL;
	}

	ddesc = pool_alloc(&usb_dev_descs);
	ddesc->dev = dev;

	return ddesc;
}

void usb_dev_desc_close(struct usb_dev_desc *ddesc) {

	pool_free(&usb_dev_descs, ddesc);
}

int usb_dev_desc_get_desc(struct usb_dev_desc *ddesc, struct usb_desc_device *desc,
		struct usb_desc_interface *idesc) {

	if (!ddesc) {
		return -EINVAL;
	}

	memcpy(desc, &ddesc->dev->dev_desc, sizeof(struct usb_desc_device));
	memcpy(idesc, &ddesc->dev->iface_desc, sizeof(struct usb_desc_device));

	return 0;
}

int usb_dev_desc_get_endp_desc(struct usb_dev_desc *ddesc, int endp,
		struct usb_desc_endpoint *desc) {

	return -ENOTSUP;

	if (!ddesc || endp > ddesc->dev->endp_n) {
		return -EINVAL;
	}

	/* desc_endpoint isn't stored in endpoint :( */
	/*memcpy(desc, &ddesc->dev->dev_desc, sizeof(struct usb_desc_device));*/

	return 0;
}

struct usb_endp_desc *usb_endp_open(struct usb_dev_desc *ddesc, int endp) {
	struct usb_endp_desc *edesc;

	if (!ddesc || endp > ddesc->dev->endp_n) {
		return NULL;
	}

	edesc = pool_alloc(&usb_endp_descs);
	edesc->endp = ddesc->dev->endpoints[endp];

	return edesc;
}

void usb_endp_desc_close(struct usb_endp_desc *edesc) {

	pool_free(&usb_endp_descs, edesc);
}

static void usb_req_notify(struct usb_request *req) {
	struct manual_event *event = req->hnd_data;

	assert(req->req_stat == USB_REQ_NOERR);

	manual_event_set_and_notify(event);
}

#if 0

int usb_read(struct usb_endp_desc *edesc, void *buf, size_t len) {
	struct usb_request *req;
	struct manual_event event;

#if 0
	if (len > edesc->endp->max_packet_size) {
		return -EMSGSIZE;
	}
#endif

	if (edesc->endp->direction != USB_DIRECTION_IN &&
			edesc->endp->direction != USB_DIRECTION_BI) {
		return -EINVAL;
	}

	req = usb_endp_request_alloc(edesc->endp, usb_req_notify, USB_TOKEN_IN,
			buf, len);
	if (!req) {
		return -ENOMEM;
	}

	manual_event_init(&event, 0);
	req->hnd_data = &event;

	usb_endp_request(edesc->endp, req);

	return manual_event_wait(&event, MANUAL_EVENT_TIMEOUT_INFINITE);
}
#endif

int usb_request(struct usb_endp_desc *edesc, void *buf, size_t len,
		usb_token_t token) {
	struct usb_request *req;
	struct manual_event event;

#if 0
	if (len > edesc->endp->max_packet_size) {
		return -EMSGSIZE;
	}
#endif

	if ((token & USB_TOKEN_OUT && token & USB_TOKEN_IN) ||
		       (!(token & USB_TOKEN_OUT) && !(token & USB_TOKEN_IN))) {
		return -EINVAL;
	}

	if (edesc->endp->direction != USB_DIRECTION_BI) {
		if (edesc->endp->direction == USB_DIRECTION_OUT &&
				token & USB_TOKEN_IN) {
			return -EINVAL;
		}
		if (edesc->endp->direction == USB_DIRECTION_IN &&
				token & USB_TOKEN_OUT) {
			return -EINVAL;
		}
	}

	req = usb_endp_request_alloc(edesc->endp, usb_req_notify,
			token, buf, len);
	if (!req) {
		return -ENOMEM;
	}

	manual_event_init(&event, 0);
	req->hnd_data = &event;

	usb_endp_request(edesc->endp, req);

	return manual_event_wait(&event, MANUAL_EVENT_TIMEOUT_INFINITE);
}
