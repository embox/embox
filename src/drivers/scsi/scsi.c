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

#include <util/log.h>

#define SCSI_CMD_MAX_LEN 16

static inline struct usb_mass *scsi2mass(struct scsi_dev *dev) {
	return member_cast_out(dev, struct usb_mass, scsi_dev);
}

static int scsi_cmd(struct scsi_dev *sdev, void *cmd, size_t cmd_len,
		void *data, size_t data_len) {
	struct usb_mass *mass = scsi2mass(sdev);
	struct scsi_cmd *scmd = cmd;
	enum usb_direction usb_dir;

	if (!sdev->attached) {
		return -ENODEV;
	}
	if (scmd->scmd_opcode == SCSI_CMD_OPCODE_WRITE10) {
		usb_dir = USB_DIRECTION_OUT;
	} else {
		usb_dir = USB_DIRECTION_IN;
	}

	log_debug("opc (0x%x) cmd_len(%d), usb_dir(%d), data_len(%d)",
		    scmd->scmd_opcode, cmd_len, usb_dir, data_len);
	
	return usb_ms_transfer(mass->usb_dev, cmd, cmd_len, usb_dir, data, data_len);
}

int scsi_do_cmd(struct scsi_dev *dev, struct scsi_cmd *cmd) {
	uint8_t scmd[SCSI_CMD_MAX_LEN];

	log_debug("opcode 0x%x, len %d", cmd->scmd_opcode, cmd->scmd_len);

	assert(cmd->scmd_len <= SCSI_CMD_MAX_LEN);
	memset(scmd + 1, 0, cmd->scmd_len - 1);
	scmd[0] = cmd->scmd_opcode;

	if (cmd->scmd_fixup) {
		cmd->scmd_fixup(scmd, dev, cmd);
	}

	return scsi_cmd(dev, scmd, cmd->scmd_len, cmd->scmd_obuf, cmd->scmd_olen);
}

static int scsi_send_inquiry(struct scsi_dev *dev) {
	struct scsi_cmd cmd = scsi_cmd_template_inquiry;
	struct scsi_data_inquiry *data;

	cmd.scmd_obuf = dev->scsi_data_scratchpad;
	cmd.scmd_olen = USB_SCSI_SCRATCHPAD_LEN;
	if (scsi_do_cmd(dev, &cmd) < 0) {
		return -1;
	}

	data = (struct scsi_data_inquiry *) dev->scsi_data_scratchpad;
	if ((data->dinq_devtype & SCSI_INQIRY_DEVTYPE_MASK)
			!= SCSI_INQIRY_DEVTYPE_BLK) {
		return -1;
	}
	return 0;
}

static int scsi_send_test_unit(struct scsi_dev *dev) {
	struct scsi_cmd cmd = scsi_cmd_template_test_unit;

	cmd.scmd_olen = 0;
	return scsi_do_cmd(dev, &cmd);
}

static int scsi_send_sense(struct scsi_dev *dev) {
	struct scsi_cmd cmd = scsi_cmd_template_sense;
	struct scsi_data_sense *data;
	uint8_t acode;

	cmd.scmd_obuf = dev->scsi_data_scratchpad;
	cmd.scmd_olen = sizeof(struct scsi_data_sense);
	if (scsi_do_cmd(dev, &cmd) < 0) {
		return -1;
	}

	data = (struct scsi_data_sense *) dev->scsi_data_scratchpad;
	acode = data->dsns_additional_code;
	if (!(acode == 0x28 || acode == 0x29)) {
		log_error("Don't know how to recover unknown error %x", acode);
		return -1;
	}
	return 0;
}

static int scsi_send_capacity10(struct scsi_dev *dev) {
	struct scsi_cmd cmd = scsi_cmd_template_cap10;
	struct scsi_data_cap10 *data;

	cmd.scmd_obuf = dev->scsi_data_scratchpad;
	cmd.scmd_olen = sizeof(struct scsi_data_cap10);
	if (scsi_do_cmd(dev, &cmd) < 0) {
		return -1;
	}

	data = (struct scsi_data_cap10 *) dev->scsi_data_scratchpad;
	dev->blk_size = be32toh(data->dc10_blklen);
	dev->blk_n = be32toh(data->dc10_lba);
	return 0;
}

int scsi_dev_attached(struct scsi_dev *dev) {
	int ret;
	int test_unit_retries = 3;

	dev->attached = 1;

	if (scsi_send_inquiry(dev)) {
		log_debug("INQUIRY failed");
		return -1;
	}

	/* FIXME This logic derived from the previos variant,
	 * but when we should issue sense? */
	while ((ret = scsi_send_test_unit(dev)) && test_unit_retries--) {
		scsi_send_sense(dev);
	}
	if (ret) {
		log_debug("TEST UNIT failed");
		return -1;
	}

	if (scsi_send_capacity10(dev)) {
		log_debug("CAPACITY10 failed");
		return -1;
	}

	scsi_disk_found(dev);

	return 0;
}

static void scsi_fixup_inquiry(void *buf, struct scsi_dev *dev,
		struct scsi_cmd *cmd) {
	struct scsi_cmd_inquiry *cmd_inquiry = buf;

	log_debug("scmd_olen %d", cmd->scmd_olen);

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

const struct scsi_cmd scsi_cmd_template_test_unit = {
	.scmd_opcode = SCSI_CMD_OPCODE_TEST_UNIT,
	.scmd_len = sizeof(struct scsi_cmd_test_unit),
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

static void scsi_dev_try_release(struct scsi_dev *dev) {
	//struct usb_dev *udev = scsi2mass(dev)->usb_dev;

	if (!dev->use_count && !dev->attached) {
		//TODO
		//usb_class_released(udev);
	}
}

void scsi_dev_detached(struct scsi_dev *dev) {

	dev->attached = 0;

	scsi_disk_lost(dev);
	scsi_dev_try_release(dev);
}

void scsi_dev_use_inc(struct scsi_dev *dev) {

	dev->use_count++;
	//usb_dev_use_inc(scsi2mass(dev)->usb_dev);
}

void scsi_dev_use_dec(struct scsi_dev *dev) {
	//struct usb_dev *udev = scsi2mass(dev)->usb_dev;

	//usb_dev_use_dec(udev);
	dev->use_count--;

	scsi_dev_try_release(dev);
}
