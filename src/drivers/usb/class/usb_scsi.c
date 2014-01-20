/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.01.2014
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <endian.h>
#include <drivers/usb/usb.h>

#include "usb_mass_storage.h"

#define USB_MS_MIGHTY_TAG 0xff00ff00

#define USB_CBW_SIGNATURE 0x43425355
#define USB_CBW_FLAGS_IN  0x80
#define USB_CBW_FLAGS_OUT 0x00

#define USB_CSW_SIGNATURE 0x53425355

#define SCSI_CMD_OPCODE_INQUIRY 0x12

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


static int scsi_cmd_inquiry_prepare(void *cmd_buf, size_t cmd_buf_len,
		size_t alloc_len) {
	struct scsi_cmd_inquiry *cmd_inquiry;

	if (cmd_buf_len < sizeof(struct scsi_cmd_inquiry)) {
		return -ERANGE;
	}

	cmd_inquiry = cmd_buf;
	cmd_inquiry->sinq_opcode = SCSI_CMD_OPCODE_INQUIRY;
	cmd_inquiry->sinq_flags = 0;
	cmd_inquiry->sinq_page_code = 0;
	cmd_inquiry->sinq_alloc_length = htobe16(alloc_len);
	cmd_inquiry->sinq_control = 0;

	return sizeof(struct scsi_cmd_inquiry);
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
		assert(req_ctx->csw.csw_status == 0);

		req_ctx->holded_hnd(req, arg);
		break;
	}
}

static int usb_scsi_transfer(struct usb_dev *dev, void *scsi_cmd,
		size_t scsi_len, enum usb_direction dir, void *buf, size_t len,
	       	usb_request_notify_hnd_t notify_hnd) {
	struct usb_mass *mass = usb2massdata(dev);
	struct usb_mass_request_ctx *req_ctx;
	int ret;

	req_ctx = &mass->req_ctx;
	req_ctx->dir = dir;
	req_ctx->buf = buf;
	req_ctx->len = len;
	req_ctx->holded_hnd = notify_hnd;
	req_ctx->req_state = USB_MASS_REQST_CBW;

	ret = usb_cbw_fill(&req_ctx->cbw, USB_MS_MIGHTY_TAG, len,
			dir, 0, scsi_cmd, scsi_len);
	if (ret < 0) {
		return ret;
	}

	return usb_endp_bulk(dev->endpoints[mass->blkout], usb_ms_transfer_done,
			&req_ctx->cbw, sizeof(struct usb_mscbw));
}

static void usb_scsi_inquire_done(struct usb_request *req, void *arg) {

}

void usb_scsi_handle(struct usb_dev *dev) {
	struct usb_mass *mass = usb2massdata(dev);
	int scsi_cmd_len, ret;
	uint8_t scsi_cmd[16];

	scsi_cmd_len = scsi_cmd_inquiry_prepare(scsi_cmd, 16,
			sizeof(mass->data_inquiry));
	assert(scsi_cmd_len >= 0);

	ret = usb_scsi_transfer(dev, scsi_cmd, scsi_cmd_len, USB_DIRECTION_IN,
			&mass->data_inquiry, sizeof(mass->data_inquiry),
			usb_scsi_inquire_done);
	assert(ret >= 0);
}

