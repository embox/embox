/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.01.2014
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <drivers/usb/usb.h>

#include <drivers/usb/class/usb_mass_storage.h>
#include <drivers/scsi.h>

#define USB_MS_MIGHTY_TAG 0xff00ff00

#define USB_CBW_SIGNATURE 0x43425355
#define USB_CBW_FLAGS_IN  0x80
#define USB_CBW_FLAGS_OUT 0x00

#define USB_CSW_SIGNATURE 0x53425355

EMBOX_UNIT_INIT(usb_mass_init);

POOL_DEF(usb_mass_classes, struct usb_mass, USB_MASS_MAX_DEVS);

static int usb_cbw_fill(struct usb_mscbw *cbw, uint32_t tag, uint32_t tr_len,
		enum usb_direction dir, uint8_t lun, void *cb, size_t len) {

	cbw->cbw_signature = USB_CBW_SIGNATURE;
	cbw->cbw_tag = tag;
	cbw->cbw_transfer_len = tr_len;
	cbw->cbw_flags = dir == USB_DIRECTION_IN ? USB_CBW_FLAGS_IN : USB_CBW_FLAGS_OUT;
	cbw->cbw_lun = lun;

	memcpy(cbw->cbw_cb, cb, len);
	memset(cbw->cbw_cb + len, 0, USB_CBW_CB_MAXLEN - len);
	cbw->cbw_len = len;

	return cbw->cbw_transfer_len;
}

static void usb_ms_transfer_done(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_mass *mass = usb2massdata(dev);
	struct usb_mass_request_ctx *req_ctx;
	int data_endp_i;

	req_ctx = &mass->req_ctx;

	switch(req_ctx->req_state) {
	case USB_MASS_REQST_CBW:
		data_endp_i = req_ctx->dir == USB_DIRECTION_IN ? mass->blkin
			: mass->blkout;
		req_ctx->req_state = USB_MASS_REQST_DATA;
		usb_endp_bulk(dev->endpoints[data_endp_i], usb_ms_transfer_done,
				req_ctx->buf, req_ctx->len);
		break;
	case USB_MASS_REQST_DATA:
		req_ctx->req_state = USB_MASS_REQST_CSW;
		usb_endp_bulk(dev->endpoints[mass->blkin], usb_ms_transfer_done,
				&req_ctx->csw, sizeof(struct usb_mscsw));
		break;
	case USB_MASS_REQST_CSW:
		assert(req_ctx->csw.csw_signature == USB_CSW_SIGNATURE);
		assert(req_ctx->csw.csw_tag == USB_MS_MIGHTY_TAG);

		req_ctx->holded_hnd(req, &req_ctx->csw);
		break;
	}
}

int usb_ms_transfer(struct usb_dev *dev, void *ms_cmd,
		size_t ms_cmd_len, enum usb_direction dir, void *buf, size_t len,
	       	usb_request_notify_hnd_t notify_hnd) {
	struct usb_mass *mass = usb2massdata(dev);
	struct usb_mass_request_ctx *req_ctx;
	int res;

	req_ctx = &mass->req_ctx;
	req_ctx->dir = dir;
	req_ctx->buf = buf;
	req_ctx->len = len;
	req_ctx->holded_hnd = notify_hnd;
	req_ctx->req_state = USB_MASS_REQST_CBW;

	res = usb_cbw_fill(&req_ctx->cbw, USB_MS_MIGHTY_TAG, len,
			dir, 0, ms_cmd, ms_cmd_len);
	if (res < 0) {
		return res;
	}

	return usb_endp_bulk(dev->endpoints[mass->blkout], usb_ms_transfer_done,
			&req_ctx->cbw, sizeof(struct usb_mscbw));
}

static void *usb_class_mass_alloc(struct usb_class *cls, struct usb_dev *dev) {
	struct usb_mass *mass = pool_alloc(&usb_mass_classes);

	if (mass) {
		mass->usb_dev = dev;
	}
	return mass;
}

static void usb_class_mass_free(struct usb_class *cls, struct usb_dev *dev, void *spec) {
	pool_free(&usb_mass_classes, spec);
}

static void usb_mass_maxlun_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_mass *mass = usb2massdata(dev);

	scsi_dev_init(&mass->scsi_dev);
	scsi_dev_attached(&mass->scsi_dev);
}

#if 0
static void usb_mass_reset_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_mass *mass = usb2massdata(dev);

	usb_endp_control(dev->endpoints[0], usb_mass_maxlun_hnd, NULL,
			USB_DEV_REQ_TYPE_RD | USB_DEV_REQ_TYPE_CLS
				| USB_DEV_REQ_TYPE_IFC,
			USB_REQ_MASS_MAXLUN, 0,
			dev->iface_desc.b_interface_number, 1,
			&mass->maxlun);
}
#endif

static void usb_mass_handle(struct usb_class *cls, struct usb_dev *dev) {
	struct usb_mass *mass = usb2massdata(dev);
	int i;

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

#if 0
	usb_endp_control(dev->endpoints[0], usb_mass_reset_hnd, NULL,
			USB_DEV_REQ_TYPE_WR | USB_DEV_REQ_TYPE_CLS
				| USB_DEV_REQ_TYPE_IFC,
			USB_REQ_MASS_RESET, 0,
			dev->iface_desc.b_interface_number, 0, NULL);
#endif
	usb_endp_control(dev->endpoints[0], usb_mass_maxlun_hnd, NULL,
			USB_DEV_REQ_TYPE_RD | USB_DEV_REQ_TYPE_CLS
				| USB_DEV_REQ_TYPE_IFC,
			USB_REQ_MASS_MAXLUN, 0,
			dev->iface_desc.b_interface_number, 1,
			&mass->maxlun);
}

static void usb_mass_release(struct usb_class *cls, struct usb_dev *dev) {
	struct usb_mass *mass = usb2massdata(dev);

	scsi_dev_detached(&mass->scsi_dev);

	/*usb_class_released(dev);*/
}

static struct usb_class usb_class_mass = {
	.usb_class = USB_CLASS_MASS,
	.class_alloc = usb_class_mass_alloc,
	.class_free = usb_class_mass_free,
	.get_conf = usb_class_generic_get_conf,
	.get_conf_hnd = usb_class_generic_get_conf_hnd,
	.class_handle = usb_mass_handle,
	.class_release = usb_mass_release,
};

static int usb_mass_init(void) {

	return usb_class_register(&usb_class_mass);
}
