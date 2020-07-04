/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    10.06.2020
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <drivers/usb/usb_driver.h>
#include <drivers/usb/usb.h>
#include <util/log.h>

#include <drivers/usb/class/ccid.h>

#define CCID_MAX_DEVS  OPTION_GET(NUMBER, ccid_max_devs)

#define CCID_BULK_TIMEOUT   1000

struct ccid {
	struct usb_interface *usb_dev;
	char ep_bulk_in;  /* Mandatory EP */
	char ep_bulk_out; /* Mandatory EP */
	char ep_intr;     /* Optional EP */
};

EMBOX_UNIT_INIT(ccid_init);

POOL_DEF(ccid_classes, struct ccid, CCID_MAX_DEVS);

static int ccid_do_handle_msg(struct ccid *ccid,
		const void *out, int out_cnt, void *in, int in_cnt) {
	int res;

	res = usb_endp_bulk_wait(ccid->usb_dev->endpoints[ccid->ep_bulk_out],
			(void *) out, out_cnt, CCID_BULK_TIMEOUT);
	if (res < 0) {
		goto out_err;
	}

	res = usb_endp_bulk_wait(ccid->usb_dev->endpoints[ccid->ep_bulk_in],
			in, in_cnt, CCID_BULK_TIMEOUT);
	if (res < 0) {
		goto out_err;
	}

	return 0;

out_err:
	log_error("failed");
	return res;
}

int ccid_handle_msg(struct usb_interface *udev, const void *out,
		void *in, int in_cnt) {
	struct ccid_msg_hdr *ccid_hdr = (struct ccid_msg_hdr *) out;
	struct ccid *ccid = (struct ccid *) udev->driver_data;
	int res = 0;

	switch (ccid_hdr->b_message_type) {
	case CCID_MESSAGE_TYPE_PC_to_RDR_IccPowerOn:
	case CCID_MESSAGE_TYPE_PC_to_RDR_GetSlotStatus:
		res = ccid_do_handle_msg(ccid, out, CCID_MSG_HEADER_SIZE,
				in, in_cnt);
		break;
	case CCID_MESSAGE_TYPE_PC_to_RDR_XfrBlock:
		res = ccid_do_handle_msg(ccid, out,
				CCID_MSG_HEADER_SIZE + ccid_hdr->dw_length,
			    in, in_cnt);
		break;
	case CCID_MESSAGE_TYPE_PC_to_RDR_IccPowerOff:
	case CCID_MESSAGE_TYPE_PC_to_RDR_GetParameters:
	case CCID_MESSAGE_TYPE_PC_to_RDR_ResetParameters:
	case CCID_MESSAGE_TYPE_PC_to_RDR_SetParameters:
	case CCID_MESSAGE_TYPE_PC_to_RDR_Escape:
	case CCID_MESSAGE_TYPE_PC_to_RDR_IccClock:
	case CCID_MESSAGE_TYPE_PC_to_RDR_T0APDU:
	case CCID_MESSAGE_TYPE_PC_to_RDR_Secure:
	case CCID_MESSAGE_TYPE_PC_to_RDR_Mechanical:
	case CCID_MESSAGE_TYPE_PC_to_RDR_Abort:
	case CCID_MESSAGE_TYPE_PC_to_RDR_SetDataRateAndClockFrequency:
	default:
		log_error("Unsupported request 0x%02x\n", ccid_hdr->b_message_type);
		res = -1;
		break;
	}

	return res;
}

int ccid_read_msg_data(struct usb_interface *udev, void *in, int in_cnt) {
	struct ccid *ccid = (struct ccid *) udev->driver_data;

	return usb_endp_bulk_wait(udev->endpoints[ccid->ep_bulk_in],
			in, in_cnt, CCID_BULK_TIMEOUT);
}

static int ccid_probe(struct usb_interface *udev) {
	struct ccid *ccid;
	int i;

	assert(udev);

	ccid = pool_alloc(&ccid_classes);
	if (!ccid) {
		return -1;
	}
	ccid->usb_dev = udev;
	udev->driver_data = ccid;

	/* Interrupt endpoint is currenly unsupported. */
	ccid->ep_intr = -1;

	for (i = 1; i <= udev->endp_n; i++) {
		struct usb_endp *endp = udev->endpoints[i];

		if (endp->type == USB_COMM_BULK) {
			if (endp->direction == USB_DIRECTION_IN) {
				ccid->ep_bulk_in = i;
			}
			if (endp->direction == USB_DIRECTION_OUT) {
				ccid->ep_bulk_out = i;
			}
		}
	}

	return 0;
}

static void ccid_disconnect(struct usb_interface *dev, void *data) {
}

static struct usb_device_id ccid_id_table[] = {
	{USB_CLASS_CCID, 0xffff, 0xffff},
	{ },
};

static struct usb_driver ccid_driver = {
	.name = "ccid",
	.probe = ccid_probe,
	.disconnect = ccid_disconnect,
	.id_table = ccid_id_table,
};

static int ccid_init(void) {
	return usb_driver_register(&ccid_driver);
}
