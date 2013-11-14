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

static void usb_req_notify(struct usb_request *req) {
	struct manual_event *event = req->hnd_data;

	assert(req->req_stat == USB_REQ_NOERR);

	manual_event_set_and_notify(event);
}

int usb_request(struct usb_dev_desc *ddesc, int endp_n, usb_token_t token,
		void *buf, size_t len) {
	struct usb_request *req;
	struct usb_endp *endp;
	struct manual_event event;

	if ((token & USB_TOKEN_OUT && token & USB_TOKEN_IN) ||
		       (!(token & USB_TOKEN_OUT) && !(token & USB_TOKEN_IN))) {
		return -EINVAL;
	}

	if (endp_n > ddesc->dev->endp_n) {
		return -EINVAL;
	}

	endp = ddesc->dev->endpoints[endp_n];

	if (endp->direction != USB_DIRECTION_BI) {
		if (endp->direction == USB_DIRECTION_OUT &&
				token & USB_TOKEN_IN) {
			return -EINVAL;
		}
		if (endp->direction == USB_DIRECTION_IN &&
				token & USB_TOKEN_OUT) {
			return -EINVAL;
		}
	}

	req = usb_endp_request_alloc(endp, usb_req_notify,
			token, buf, len);
	if (!req) {
		return -ENOMEM;
	}

	manual_event_init(&event, 0);
	req->hnd_data = &event;

	usb_endp_request(endp, req);

	return manual_event_wait(&event, MANUAL_EVENT_TIMEOUT_INFINITE);
}
