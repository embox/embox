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
#include <drivers/usb/class/usb_mass_storage.h>

#include <drivers/scsi.h>

static inline struct usb_mass *scsi2mass(struct scsi_dev *dev) {
	return member_cast_out(dev, struct usb_mass, scsi_dev);
}

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
	struct usb_mass *mass = scsi2mass(sdev);
	struct scsi_cmd *scmd = cmd;
	enum usb_direction usb_dir = USB_DIRECTION_IN;

	if (!sdev->attached) {
		return -ENODEV;
	}
	if(scmd->scmd_opcode == 0x2a) {
		usb_dir = USB_DIRECTION_OUT;
	} else {
		usb_dir = USB_DIRECTION_IN;
	}

	return usb_ms_transfer(mass->usb_dev, cmd, cmd_len, usb_dir, data, data_len,
			usb_scsi_notify);
}

#define SCSI_CMD_LEN 16
int scsi_do_cmd(struct scsi_dev *dev, struct scsi_cmd *cmd) {
	uint8_t scmd[SCSI_CMD_LEN];

	assert(cmd->scmd_len <= SCSI_CMD_LEN);
	memset(scmd + 1, 0, cmd->scmd_len - 1);
	scmd[0] = cmd->scmd_opcode;

	if (cmd->scmd_fixup) {
		cmd->scmd_fixup(scmd, dev, cmd);
	}

	return scsi_cmd(dev, scmd, cmd->scmd_len, cmd->scmd_obuf,
			cmd->scmd_olen);
}

static void scsi_fixup_inquiry(void *buf, struct scsi_dev *dev,
		struct scsi_cmd *cmd) {
	struct scsi_cmd_inquiry *cmd_inquiry = buf;
	cmd_inquiry->sinq_alloc_length = htobe16(cmd->scmd_olen);
}

const struct scsi_cmd scsi_cmd_template_inquiry = {
	.scmd_opcode = SCSI_CMD_OPCODE_INQUIRY,
	.scmd_len = sizeof(struct scsi_cmd_inquiry),
	.scmd_fixup = scsi_fixup_inquiry,
};

const struct scsi_cmd scsi_cmd_template_cap10 = {
	.scmd_opcode = SCSI_CMD_OPCODE_CAP10,
	.scmd_len = sizeof(struct scsi_cmd_cap10),
};

static void scsi_fixup_read_sense(void *buf, struct scsi_dev *dev,
		struct scsi_cmd *cmd) {
	struct scsi_cmd_sense *cmd_sense = buf;
	cmd_sense->ssns_alloc_length = cmd->scmd_olen;
}

const struct scsi_cmd scsi_cmd_template_sense = {
	.scmd_opcode = SCSI_CMD_OPCODE_SENSE,
	.scmd_len = sizeof(struct scsi_cmd_sense),
	.scmd_fixup = scsi_fixup_read_sense,
};

static void scsi_fixup_read10(void *buf, struct scsi_dev *dev,
		struct scsi_cmd *cmd) {
	struct scsi_cmd_read10 *rcmd = buf;
	const unsigned int blk_size = dev->blk_size;

	assert(blk_size > 0);

	rcmd->sr10_lba = htobe32(cmd->scmd_lba);
	rcmd->sr10_transfer_len = htobe16(cmd->scmd_olen / blk_size);
}

const struct scsi_cmd scsi_cmd_template_read10 = {
	.scmd_opcode = SCSI_CMD_OPCODE_READ10,
	.scmd_len = sizeof(struct scsi_cmd_read10),
	.scmd_fixup = scsi_fixup_read10,
};

static void scsi_fixup_write10(void *buf, struct scsi_dev *dev,
		struct scsi_cmd *cmd) {
	struct scsi_cmd_write10 *wcmd = buf;
	const unsigned int blk_size = dev->blk_size;

	assert(blk_size > 0);

	wcmd->sw10_lba = htobe32(cmd->scmd_lba);
	wcmd->sw10_transfer_len = htobe16(cmd->scmd_olen / blk_size);
}

const struct scsi_cmd scsi_cmd_template_write10 = {
	.scmd_opcode = SCSI_CMD_OPCODE_WRITE10,
	.scmd_len = sizeof(struct scsi_cmd_write10),
	.scmd_fixup = scsi_fixup_write10,
};

int scsi_dev_init(struct scsi_dev *dev) {
	return 0;
}

void scsi_state_transit(struct scsi_dev *dev,
		const struct scsi_dev_state *to) {
	const struct scsi_dev_state *from = dev->state;

	if (from && from->sds_leave)
		from->sds_leave(dev);

	if (to->sds_enter)
		to->sds_enter(dev);

	dev->state = to;
}

static const struct scsi_dev_state scsi_state_inquiry;
static const struct scsi_dev_state scsi_state_capacity;
static const struct scsi_dev_state scsi_state_sense;

static void scsi_inquiry_enter(struct scsi_dev *dev) {
	struct scsi_cmd cmd = scsi_cmd_template_inquiry;
	cmd.scmd_obuf = dev->scsi_data_scratchpad;
	cmd.scmd_olen = sizeof(struct scsi_data_inquiry);

	scsi_do_cmd(dev, &cmd);
}

static void scsi_inquiry_input(struct scsi_dev *dev, int res) {
	struct scsi_data_inquiry *data;

	assert(res == 0);

	data = (struct scsi_data_inquiry *) dev->scsi_data_scratchpad;
	if ((data->dinq_devtype & SCSI_INQIRY_DEVTYPE_MASK)
			!= SCSI_INQIRY_DEVTYPE_BLK) {
		return;
	}

	scsi_state_transit(dev, &scsi_state_capacity);
}

static const struct scsi_dev_state scsi_state_inquiry = {
	.sds_enter = scsi_inquiry_enter,
	.sds_input = scsi_inquiry_input,
};

static void scsi_capacity_enter(struct scsi_dev *dev) {
	struct scsi_cmd cmd = scsi_cmd_template_cap10;
	cmd.scmd_obuf = dev->scsi_data_scratchpad;
	cmd.scmd_olen = sizeof(struct scsi_data_cap10);

	scsi_do_cmd(dev, &cmd);
}

static void scsi_capacity_input(struct scsi_dev *dev, int res) {
	struct scsi_data_cap10 *data;

	if (res < 0) {
		scsi_dev_recover(dev);
		return;
	}

	data = (struct scsi_data_cap10 *) dev->scsi_data_scratchpad;
	dev->blk_size = be32toh(data->dc10_blklen);
	dev->blk_n = be32toh(data->dc10_lba);

	scsi_disk_found(dev);
}

static const struct scsi_dev_state scsi_state_capacity = {
	.sds_enter = scsi_capacity_enter,
	.sds_input = scsi_capacity_input,
};

static void scsi_sense_enter(struct scsi_dev *dev) {
	struct scsi_cmd cmd = scsi_cmd_template_sense;
	cmd.scmd_obuf = dev->scsi_data_scratchpad;
	cmd.scmd_olen = sizeof(struct scsi_data_sense);

	scsi_do_cmd(dev, &cmd);
}

static void scsi_sense_input(struct scsi_dev *dev, int res) {
	struct scsi_data_sense *data;
	uint8_t acode;

	assert(res == 0);

	data = (struct scsi_data_sense *) dev->scsi_data_scratchpad;
	acode = data->dsns_additional_code;
	assertf(acode == 0x28 || acode == 0x29, "Don't know how to recover "
			"unknown error %x", acode);

	/* 0x28 and 0x29 are just required attention, seems that can go on */

	scsi_state_transit(dev, dev->holded_state);
	dev->holded_state = NULL;
}

static const struct scsi_dev_state scsi_state_sense = {
	.sds_enter = scsi_sense_enter,
	.sds_input = scsi_sense_input,
};

static void scsi_dev_try_release(struct scsi_dev *dev) {
	struct usb_dev *udev = scsi2mass(dev)->usb_dev;

	if (!dev->use_count && !dev->attached) {
		usb_class_released(udev);
	}
}

void scsi_dev_recover(struct scsi_dev *dev) {

	assertf(dev->holded_state == NULL, "Can't recover recovering procedure");

	dev->holded_state = dev->state;

	scsi_state_transit(dev, &scsi_state_sense);
}

void scsi_dev_attached(struct scsi_dev *dev) {

	dev->attached = 1;

	dev->state = dev->holded_state = NULL;
	scsi_state_transit(dev, &scsi_state_inquiry);
}

void scsi_dev_detached(struct scsi_dev *dev) {

	dev->attached = 0;

	scsi_disk_lost(dev);
	scsi_dev_try_release(dev);
}

void scsi_request_done(struct scsi_dev *dev, int res) {

	if (dev->state && dev->state->sds_input) {
		dev->state->sds_input(dev, res);
	}
}

void scsi_dev_use_inc(struct scsi_dev *dev) {

	dev->use_count++;
	usb_dev_use_inc(scsi2mass(dev)->usb_dev);
}

void scsi_dev_use_dec(struct scsi_dev *dev) {
	struct usb_dev *udev = scsi2mass(dev)->usb_dev;

	usb_dev_use_dec(udev);
	dev->use_count--;

	scsi_dev_try_release(dev);
}
