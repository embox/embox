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
#include <unistd.h>

#include <endian.h>

#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <drivers/usb/usb_driver.h>
#include <drivers/usb/usb.h>
#include <drivers/usb/class/usb_mass_storage.h>
#include <drivers/scsi.h>

#include <util/log.h>

#define USB_MS_MIGHTY_TAG 0xff00ff00

#define USB_CBW_SIGNATURE 0x43425355
#define USB_CBW_FLAGS_IN  0x80
#define USB_CBW_FLAGS_OUT 0x00

#define USB_CSW_SIGNATURE 0x53425355

#define MS_BULK_TIMEOUT 1000

EMBOX_UNIT_INIT(usb_mass_init);

POOL_DEF(usb_mass_classes, struct usb_mass, USB_MASS_MAX_DEVS);

static void usb_cbw_fill(struct usb_mscbw *cbw, uint32_t tag, uint32_t tr_len,
		enum usb_direction dir, uint8_t lun, void *cb, size_t len) {

	cbw->cbw_signature = USB_CBW_SIGNATURE;
	cbw->cbw_tag = tag;
	cbw->cbw_transfer_len = tr_len;
	cbw->cbw_flags = dir == USB_DIRECTION_IN ? USB_CBW_FLAGS_IN : USB_CBW_FLAGS_OUT;
	cbw->cbw_lun = lun;

	memcpy(cbw->cbw_cb, cb, len);
	memset(cbw->cbw_cb + len, 0, USB_CBW_CB_MAXLEN - len);
	cbw->cbw_len = len;
}

int usb_ms_transfer(struct usb_dev *dev, void *ms_cmd,
		size_t ms_cmd_len, enum usb_direction dir, void *buf, size_t len) {
	struct usb_mass *mass = usb2massdata(dev);
	struct usb_endp *endp;
	struct usb_mscbw cbw;
	struct usb_mscsw csw;
	int res;

	log_debug("len=%d, dir=0x%x", len, dir);

	/* Send request */
	usb_cbw_fill(&cbw, USB_MS_MIGHTY_TAG, len,
			dir, 0, ms_cmd, ms_cmd_len);
	res = usb_endp_bulk_wait(dev->endpoints[mass->blkout],
			&cbw, sizeof(struct usb_mscbw), MS_BULK_TIMEOUT);
	if (res < 0) {
		goto failed;
	}
	if (len) {
		/* Transfer data */
		if (dir == USB_DIRECTION_IN) {
			endp = dev->endpoints[mass->blkin];
		} else if (dir == USB_DIRECTION_OUT) {
			endp = dev->endpoints[mass->blkout];
		}
		res = usb_endp_bulk_wait(endp, buf, len, MS_BULK_TIMEOUT);
		if (res < 0) {
			goto failed;
		}
	}
	/* Get status */
	endp = dev->endpoints[mass->blkin];
	memset(&csw, 0, sizeof(struct usb_mscsw));
	res = usb_endp_bulk_wait(endp, &csw, sizeof(struct usb_mscsw), MS_BULK_TIMEOUT);
	if (res < 0) {
		goto failed;
	}
	log_debug("csw_signature=0x%x; csw_tag=0x%x; "
			"csw_data_resude=0x%x; csw_status=0x%x",
			csw.csw_signature, csw.csw_tag,
			csw.csw_data_resude, csw.csw_status);
	if (csw.csw_signature != USB_CSW_SIGNATURE ||
			csw.csw_tag != USB_MS_MIGHTY_TAG) {
		log_error("CSW mismatch!");
		goto failed;
	}
	return 0;
failed:
	log_error("failed");
	return -1;
}

static void usb_mass_start(struct usb_dev *dev) {
	struct usb_mass *mass = usb2massdata(dev);
	int ret;
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

	ret = usb_endp_control_wait(dev->endpoints[0],
			USB_DIR_OUT | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_IFACE,
			USB_REQ_MASS_RESET, 0,
			dev->iface_desc.b_interface_number, 0, NULL, 1000);
	if (ret) {
		log_error("Mass storage reset error\n\n");
		return;
	}

	usleep(100000);

	ret = usb_endp_control_wait(dev->endpoints[0],
			USB_DIR_IN | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_IFACE,
			USB_REQ_MASS_MAXLUN, 0,
			dev->iface_desc.b_interface_number, 1, &mass->maxlun, 1000);
	if (ret) {
		log_error("Mass storage conftrol error\n\n");
		return;
	}
	log_debug("mass(blkin = %d, blkout = %d, maxlun=%d)", mass->blkin, mass->blkout, mass->maxlun);

	scsi_dev_init(&mass->scsi_dev);
	scsi_dev_attached(&mass->scsi_dev);
}

static int usb_ms_probe(struct usb_dev *dev) {
	struct usb_mass *mass;

	mass = pool_alloc(&usb_mass_classes);
	if (!mass) {
		return -1;
	}
	mass->usb_dev = dev;
	dev->driver_data = mass;

	usb_mass_start(dev);

	return 0;
}

static void usb_ms_disconnect(struct usb_dev *dev, void *data) {
	struct usb_mass *mass = usb2massdata(dev);
	scsi_dev_detached(&mass->scsi_dev);
	pool_free(&usb_mass_classes, mass);
}

/* TODO */
static struct usb_device_id usb_ms_id_table[] = {
	{USB_CLASS_MASS, 0xffff, 0xffff},
	{ },
};

struct usb_driver usb_driver_ms = {
	.name = "mass_storage",
	.probe = usb_ms_probe,
	.disconnect = usb_ms_disconnect,
	.id_table = usb_ms_id_table,
};

static int usb_mass_init(void) {
	return usb_driver_register(&usb_driver_ms);
}
