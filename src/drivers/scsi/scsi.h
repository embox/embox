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

struct scsi_dev_state {
	void (*sds_enter)(struct scsi_dev *sdev);
	void (*sds_input)(struct scsi_dev *sdev, int req_status);
	void (*sds_leave)(struct scsi_dev *sdev);
};

#define USB_SCSI_SCRATCHPAD_LEN 36
struct scsi_dev {
	int idx;
	const struct scsi_dev_state *state;
	const struct scsi_dev_state *holded_state;

	uint8_t scsi_data_scratchpad[USB_SCSI_SCRATCHPAD_LEN];

	unsigned int blk_size;
	unsigned int blk_n;

	struct block_dev *bdev;
	struct mutex m;
	struct waitq wq;
	char in_cmd;
	char cmd_complete;
	char attached;
	unsigned int use_count;
};

struct scsi_cmd {
	uint8_t scmd_opcode;
	size_t  scmd_len;
	void    (*scmd_fixup)(void *buf, struct scsi_dev *dev,
			struct scsi_cmd *cmd);
	void    *scmd_obuf;
	size_t  scmd_olen;

	size_t  scmd_lba;
};

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

#define SCSI_DATA_INQUIRY_VID_LEN 8
#define SCSI_DATA_INQUIRY_PID_LEN 16
#define SCSI_DATA_INQUIRY_REV_LEN 4
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
} __attribute__((packed));

struct scsi_data_cap10 {
	uint32_t dc10_lba;
	uint32_t dc10_blklen;
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

extern const struct scsi_cmd scsi_cmd_template_inquiry;
extern const struct scsi_cmd scsi_cmd_template_cap10;
extern const struct scsi_cmd scsi_cmd_template_sense;
extern const struct scsi_cmd scsi_cmd_template_read10;
extern const struct scsi_cmd scsi_cmd_template_write10;

int scsi_dev_init(struct scsi_dev *dev);
void scsi_dev_attached(struct scsi_dev *dev);
void scsi_dev_detached(struct scsi_dev *dev);
void scsi_request_done(struct scsi_dev *dev, int res);

int scsi_do_cmd(struct scsi_dev *dev, struct scsi_cmd *cmd);

void scsi_dev_recover(struct scsi_dev *dev);
void scsi_state_transit(struct scsi_dev *dev,
		const struct scsi_dev_state *to);
void scsi_dev_use_inc(struct scsi_dev *dev);
void scsi_dev_use_dec(struct scsi_dev *dev);

extern void scsi_disk_found(struct scsi_dev *dev);
extern void scsi_disk_lost(struct scsi_dev *dev);

#endif /* SCSI_H_ */

