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

int scsi_do_cmd(struct scsi_dev *sdev, struct scsi_cmd *scmd) {
	struct usb_mass *mass = scsi2mass(sdev);
	enum usb_direction usb_dir;
	uint8_t scmd_opcode;

	if (!sdev->attached) {
		return -ENODEV;
	}
	assert(scmd->scmd_len <= SCSI_CMD_MAX_LEN);
	assert(scmd->scmd_buf);

	scmd_opcode = scmd->scmd_buf[0];
	if (scmd_opcode == SCSI_CMD_OPCODE_WRITE10) {
		usb_dir = USB_DIRECTION_OUT;
	} else {
		usb_dir = USB_DIRECTION_IN;
	}
	log_debug("opcode 0x%x, usb_dir(%d), len %d",
		scmd_opcode, usb_dir, scmd->scmd_len);

	return usb_ms_transfer(mass->usb_dev, scmd->scmd_buf, scmd->scmd_len,
		       usb_dir, scmd->sdata_buf, scmd->sdata_len);
}

static int scsi_send_inquiry(struct scsi_dev *dev) {
	struct scsi_cmd cmd;
	struct scsi_cmd_inquiry cmd_inquiry;
	struct scsi_data_inquiry data;

	memset(&data, 0, sizeof data);

	cmd.scmd_buf    = (uint8_t *) &cmd_inquiry;
	cmd.scmd_len    = sizeof cmd_inquiry;
	cmd.sdata_buf   = (uint8_t *) &data;
	cmd.sdata_len   = sizeof data;

	memset(&cmd_inquiry, 0, sizeof cmd_inquiry);
	cmd_inquiry.sinq_opcode = SCSI_CMD_OPCODE_INQUIRY;
	cmd_inquiry.sinq_alloc_length = htobe16(cmd.sdata_len);

	if (scsi_do_cmd(dev, &cmd) < 0) {
		return -1;
	}
	if ((data.dinq_devtype & SCSI_INQIRY_DEVTYPE_MASK)
			!= SCSI_INQIRY_DEVTYPE_BLK) {
		return -1;
	}
	return 0;
}

static int scsi_send_test_unit(struct scsi_dev *dev) {
	struct scsi_cmd cmd;
	struct scsi_cmd_test_unit cmd_test_unit;

	memset(&cmd_test_unit, 0, sizeof cmd_test_unit);
	cmd_test_unit.scsi_cmd_opcode = SCSI_CMD_OPCODE_TEST_UNIT;

	memset(&cmd, 0, sizeof cmd);
	cmd.scmd_buf    = (uint8_t *) &cmd_test_unit;
	cmd.scmd_len    = sizeof cmd_test_unit;

	return scsi_do_cmd(dev, &cmd);
}

static int scsi_send_sense(struct scsi_dev *dev) {
	struct scsi_cmd cmd;
	struct scsi_cmd_sense cmd_sense;
	struct scsi_data_sense data;
	uint8_t acode;

	memset(&data, 0, sizeof data);

	cmd.scmd_buf    = (uint8_t *) &cmd_sense;
	cmd.scmd_len    = sizeof cmd_sense;
	cmd.sdata_buf   = (uint8_t *) &data;
	cmd.sdata_len   = sizeof data;

	memset(&cmd_sense, 0, sizeof cmd_sense);
	cmd_sense.ssns_opcode = SCSI_CMD_OPCODE_SENSE;
	cmd_sense.ssns_alloc_length = cmd.sdata_len;

	if (scsi_do_cmd(dev, &cmd) < 0) {
		return -1;
	}
	acode = data.dsns_additional_code;
	if (!(acode == 0x28 || acode == 0x29)) {
		log_error("Don't know how to recover unknown error %x", acode);
		return -1;
	}
	return 0;
}

static int scsi_send_capacity10(struct scsi_dev *dev) {
	struct scsi_cmd cmd;
	struct scsi_cmd_cap10 cmd_cap10;
	struct scsi_data_cap10 data;

	memset(&cmd_cap10, 0, sizeof cmd_cap10);
	cmd_cap10.sc10_opcode = SCSI_CMD_OPCODE_CAP10;

	cmd.scmd_buf    = (uint8_t *) &cmd_cap10;
	cmd.scmd_len    = sizeof cmd_cap10;
	cmd.sdata_buf   = (uint8_t *) &data;
	cmd.sdata_len   = sizeof data;

	if (scsi_do_cmd(dev, &cmd) < 0) {
		return -1;
	}
	dev->blk_size = be32toh(data.dc10_blklen);
	dev->blk_n = be32toh(data.dc10_lba);
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
