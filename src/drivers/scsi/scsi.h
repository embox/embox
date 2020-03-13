/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.01.2014
 */

#ifndef SCSI_H_
#define SCSI_H_

#include <stdint.h>
#include <sys/types.h>

#include <kernel/sched/waitq.h>
#include <kernel/thread/sync/mutex.h>

struct scsi_dev;

#define USB_SCSI_SCRATCHPAD_LEN 36
struct scsi_dev {
	int idx;

	unsigned int blk_size;
	unsigned int blk_n;

	struct block_dev *bdev;
	struct mutex m;
	char attached;
	unsigned int use_count;
};

struct scsi_cmd {
	uint8_t *scmd_buf;
	size_t  scmd_len;
	uint8_t *sdata_buf;
	size_t  sdata_len;
};

#define SCSI_CMD_OPCODE_TEST_UNIT 0x00
struct scsi_cmd_test_unit {
	uint8_t  scsi_cmd_opcode;
	uint8_t  __reserved[3];
	uint8_t  control;
	uint8_t  __pads[1];
} __attribute__((packed));


#define SCSI_CMD_OPCODE_INQUIRY 0x12
struct scsi_cmd_inquiry {
	uint8_t  sinq_opcode;
	uint8_t  sinq_flags;
	uint8_t  sinq_page_code;
	uint16_t sinq_alloc_length;
	uint8_t  sinq_control;
} __attribute__((packed));

#define SCSI_INQIRY_DEVTYPE_MASK  0x1f
#define SCSI_INQIRY_DEVTYPE_BLK   0x00

#define SCSI_DATA_INQUIRY_VID_LEN     8
#define SCSI_DATA_INQUIRY_PID_LEN     16
#define SCSI_DATA_INQUIRY_REV_LEN     4

#define SCSI_DATA_INQUIRY_PROTECT     0x1

struct scsi_data_inquiry {
	uint8_t dinq_devtype;
	uint8_t dinq_removable;
	uint8_t dinq_version;
	uint8_t dinq_flags1;
	uint8_t dinq_additinal_len;
	uint8_t dinq_flags2;
	uint8_t dinq_flags3;
	uint8_t dinq_flags4;
	uint8_t dinq_vendor_id[SCSI_DATA_INQUIRY_VID_LEN];
	uint8_t dinq_product_id[SCSI_DATA_INQUIRY_PID_LEN];
	uint8_t dinq_rev[SCSI_DATA_INQUIRY_REV_LEN];
} __attribute__((packed));

#define SCSI_CMD_OPCODE_CAP10 0x25
struct scsi_cmd_cap10 {
	uint8_t  sc10_opcode;
	uint8_t  sc10_obsolete;
	uint32_t sc10_block_addr;
	uint8_t  sc10_reserve1;
	uint8_t  sc10_reserve2;
	uint8_t  sc10_pmi;
	uint8_t  sc10_control;
	uint8_t __sc10_padd[2];
} __attribute__((packed));

struct scsi_data_cap10 {
	uint32_t dc10_lba;
	uint32_t dc10_blklen;
} __attribute__((packed));

#define SCSI_CMD_OPCODE_CAP16 0x9E
struct scsi_cmd_cap16 {
	uint8_t  sc16_opcode;
	uint8_t  sc16_service_action;
	uint64_t sc16_block_addr;
	uint32_t sc16_alloc_len;
	uint8_t  sc16_pmi;
	uint8_t  sc16_control;
} __attribute__((packed));

struct scsi_data_cap16 {
	uint64_t dc16_lba;
	uint32_t dc16_blklen;
	uint8_t dc16_flags;
	uint8_t dc16_exponent;
	uint8_t dc16_aligned_lba;
	uint8_t dc16_reserved[0x10];
} __attribute__((packed));

#define SCSI_CMD_OPCODE_SENSE 0x03
struct scsi_cmd_sense {
	uint8_t  ssns_opcode;
	uint8_t  ssns_desc;
	uint8_t  ssns_reserve1;
	uint8_t  ssns_reserve2;
	uint8_t  ssns_alloc_length;
	uint8_t  ssns_control;
} __attribute__((packed));

#define SCSI_DATA_SENSE_KEY_MASK 0xf
struct scsi_data_sense {
	uint8_t  dsns_response;
	uint8_t  dsns_obsolete;
	uint8_t  dsns_key;
	uint32_t dnsn_information;
	uint8_t  dsns_additional_len;
	uint32_t dnsn_spec_information;
	uint8_t  dsns_additional_code;
	uint8_t  dsns_additional_qualifier;
	uint8_t  dsns_unit_code;
	uint8_t  dsns_key_specific1;
	uint8_t  dsns_key_specific2;
	uint8_t  dsns_key_specific3;
} __attribute__((packed));

#define SCSI_CMD_OPCODE_READ10 0x28
struct scsi_cmd_read10 {
	uint8_t  sr10_opcode;
	uint8_t  sr10_flags;
	uint32_t sr10_lba;
	uint8_t  sr10_grpnum;
	uint16_t sr10_transfer_len;
	uint8_t  sr10_control;
} __attribute__((packed));


#define SCSI_CMD_OPCODE_WRITE10 0x2A
struct scsi_cmd_write10 {
	uint8_t  sw10_opcode;
	uint8_t  sw10_flags;
	uint32_t sw10_lba;
	uint8_t  sw10_grpnum;
	uint16_t sw10_transfer_len;
	uint8_t  sw10_control;
} __attribute__((packed));

extern int scsi_dev_attached(struct scsi_dev *dev);
extern void scsi_dev_detached(struct scsi_dev *dev);
extern void scsi_disk_bdev_try_unbind(struct scsi_dev *sdev);
extern int scsi_do_cmd(struct scsi_dev *dev, struct scsi_cmd *cmd);
extern void scsi_dev_use_inc(struct scsi_dev *dev);
extern void scsi_dev_use_dec(struct scsi_dev *dev);

extern void scsi_disk_found(struct scsi_dev *dev);
extern void scsi_disk_lost(struct scsi_dev *dev);

#endif /* SCSI_H_ */

