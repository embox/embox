/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.01.2014
 */

#include <errno.h>
#include <endian.h>
#include <string.h>
#include <drivers/usb/usb.h>
#include "usb_mass_storage.h"

#include "scsi.h"

static void usb_scsi_notify(struct usb_request *req, void *arg) {
	struct usb_mscsw *csw = arg;
	struct scsi_dev *sdev = &(usb2massdata(req->endp->dev)->scsi_dev);

	if (csw->csw_status != 0) {
		assert(csw->csw_status == 1);
		scsi_request_done(sdev, -1);
		return;
	}

	scsi_request_done(sdev, 0);
}

int scsi_cmd(struct scsi_dev *sdev, void *cmd, size_t cmd_len, void *data, size_t data_len) {
	struct usb_mass *mass = member_cast_out(sdev, struct usb_mass, scsi_dev);

	return usb_ms_transfer(mass->usb_dev, cmd, cmd_len, USB_DIRECTION_IN, data, data_len,
			usb_scsi_notify);
}

int scsi_cmd_inquiry_prepare(void *cmd_buf, size_t cmd_buf_len,
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

int scsi_cmd_read_capacity10_prepare(void *cmd_buf, size_t cmd_buf_len) {
	struct scsi_cmd_cap10 *cmd_cap10;

	if (cmd_buf_len < sizeof(struct scsi_cmd_cap10)) {
		return -ERANGE;
	}

	cmd_cap10 = cmd_buf;
	memset(cmd_cap10, 0, sizeof(struct scsi_cmd_cap10));
	cmd_cap10->sc10_opcode = SCSI_CMD_OPCODE_READ_CAP10;

	return sizeof(struct scsi_cmd_cap10);
}

int scsi_cmd_request_sense_prepare(void *cmd_buf, size_t cmd_buf_len,
		size_t alloc_len) {
	struct scsi_cmd_sense *cmd_sense;

	if (cmd_buf_len < sizeof(struct scsi_cmd_sense)) {
		return -ERANGE;
	}

	cmd_sense = cmd_buf;
	memset(cmd_sense, 0, sizeof(struct scsi_cmd_sense));
	cmd_sense->ssns_opcode = SCSI_CMD_OPCODE_REQ_SENSE;
	cmd_sense->ssns_alloc_length = alloc_len;

	return sizeof(struct scsi_cmd_sense);
}

#if 0
static void usb_scsi_read_capacity_done(struct usb_request *req, void *arg);
static void usb_scsi_inquire_done(struct usb_request *req, void *arg);

static void usb_scsi_inquire_done(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_mass *mass = usb2massdata(dev);
	struct usb_mass_request_ctx *req_ctx;
	struct scsi_data_inquiry *data;
	uint8_t scsi_cmd[SCSI_CMD_LEN];
	int scsi_cmd_len, res;

	req_ctx = &mass->req_ctx;
	data = req_ctx->buf;

	if ((data->dinq_devtype & SCSI_INQIRY_DEVTYPE_MASK)
			!= SCSI_INQIRY_DEVTYPE_BLK) {
		return;
	}

	assert(res >= 0);
}
#endif

#define SCSI_CMD_LEN 16
static int scsi_do_inquiry(struct scsi_dev *dev) {
	uint8_t scmd[SCSI_CMD_LEN];
	int scsi_cmd_len;

	static_assert(USB_SCSI_SCRATCHPAD_LEN >= sizeof(struct scsi_data_inquiry));

	scsi_cmd_len = scsi_cmd_inquiry_prepare(scmd, SCSI_CMD_LEN,
			sizeof(struct scsi_data_inquiry));
	if (scsi_cmd_len < 0) {
		return scsi_cmd_len;
	}

	return scsi_cmd(dev, scmd, scsi_cmd_len, dev->scsi_data_scratchpad,
			sizeof(struct scsi_data_inquiry));
}

static int scsi_do_read_cap(struct scsi_dev *dev) {
	uint8_t scmd[SCSI_CMD_LEN];
	int scsi_cmd_len;

	scsi_cmd_len = scsi_cmd_read_capacity10_prepare(scmd, SCSI_CMD_LEN);
	if (scsi_cmd_len < 0) {
		return scsi_cmd_len;
	}

	return scsi_cmd(dev, scmd, scsi_cmd_len, dev->scsi_data_scratchpad,
			sizeof(struct scsi_data_cap10));
}

struct scsi_mstate {
	int (*action)(struct scsi_dev *dev);
} scsi_mstates[] = {
	[SCSI_ATTACHED]  = { scsi_do_inquiry, },
	[SCSI_QUIRED]    = { scsi_do_read_cap, },
	[SCSI_CAPACITED] = { NULL },
};

int scsi_dev_init(struct scsi_dev *dev) {

	return 0;
}

static void scsi_machine_do(struct scsi_dev *dev) {
	int res;
	struct scsi_mstate *mstate;
	uint8_t scmd[SCSI_CMD_LEN];

	if (dev->inerror) {
		res = scsi_cmd(dev, scmd,
				scsi_cmd_request_sense_prepare(scmd, SCSI_CMD_LEN,
					sizeof(struct scsi_data_sense)),
				dev->scsi_data_scratchpad, sizeof(struct scsi_data_sense));

	} else {
		mstate = &scsi_mstates[dev->state];
		assert(mstate->action);
		res = mstate->action(dev);
	}

	assert(res >= 0);
}

void scsi_dev_attached(struct scsi_dev *dev) {

	dev->state = SCSI_ATTACHED;
	dev->inerror = 0;
	scsi_machine_do(dev);
}

void scsi_request_done(struct scsi_dev *dev, int res) {
	static int was_error = 0;

	if (res < 0) {
		dev->inerror = 1;
		was_error = 1;
	} else {
		if (!was_error) {
			dev->state++;
		} else {
			struct scsi_data_sense *sense =
				(struct scsi_data_sense *) dev->scsi_data_scratchpad;

			sense = sense;

			while(1);
		}
	}

	scsi_machine_do(dev);
}
