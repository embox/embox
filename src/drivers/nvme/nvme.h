/**
 * @file
 * @brief Minimal NVMe 1.1+ register and data structure definitions
 *        used by the polling driver.
 */

#ifndef NVME_H_
#define NVME_H_

#include <stdint.h>

/* Controller registers, BAR0 offsets */
#define NVME_CAP   0x000 /* 64 bit */
#define NVME_VS    0x008
#define NVME_INTMS 0x00c
#define NVME_INTMC 0x010
#define NVME_CC    0x014
#define NVME_CSTS  0x01c
#define NVME_AQA   0x024
#define NVME_ASQ   0x028 /* 64 bit */
#define NVME_ACQ   0x030 /* 64 bit */

#define NVME_CAP_MQES(cap)   ((cap)&0xffff)
#define NVME_CAP_TO(cap)     (((cap) >> 24) & 0xff) /* in 500 ms units */
#define NVME_CAP_DSTRD(cap)  (((cap) >> 32) & 0xf)
#define NVME_CAP_MPSMIN(cap) (((cap) >> 48) & 0xf)

#define NVME_CC_EN       (1u << 0)
#define NVME_CC_CSS_NVM  (0u << 1)
#define NVME_CC_MPS(mps) ((mps) << 4)
#define NVME_CC_IOSQES6  (0x6u << 16)
#define NVME_CC_IOCQES4  (0x4u << 20)

#define NVME_CSTS_RDY (1u << 0)
#define NVME_CSTS_CFS (1u << 1)

/* Doorbell of queue qid: submit queue tail / completion queue head */
#define NVME_DBR_SQ(base, stride, qid) \
	((base) + 0x1000 + (2 * (qid)) * (stride))
#define NVME_DBR_CQ(base, stride, qid) \
	((base) + 0x1000 + (2 * (qid) + 1) * (stride))

/* Admin command opcodes */
#define NVME_OPC_DELETE_SQ 0x00
#define NVME_OPC_CREATE_SQ 0x01
#define NVME_OPC_DELETE_CQ 0x04
#define NVME_OPC_CREATE_CQ 0x05
#define NVME_OPC_IDENTIFY  0x06

/* I/O command opcodes */
#define NVME_OPC_IO_WRITE 0x01
#define NVME_OPC_IO_READ  0x02

/* Generic status: [0] phase, [8:1] SC, [11:9] SCT */
#define NVME_CQE_PHASE(cqe) ((cqe)->status & 1)
#define NVME_CQE_OK(cqe)    ((((cqe)->status >> 1) & 0xff) == 0)

struct nvme_command {
	uint8_t opcode;
	uint8_t flags;
	uint16_t cid;
	uint32_t nsid;
	uint64_t reserved;
	uint64_t mptr;
	uint64_t prp1;
	uint64_t prp2;
	uint32_t cdw10;
	uint32_t cdw11;
	uint32_t cdw12;
	uint32_t cdw13;
	uint32_t cdw14;
	uint32_t cdw15;
}; /* 64 bytes */

struct nvme_completion {
	uint32_t result;
	uint32_t reserved;
	uint16_t sq_head;
	uint16_t sq_id;
	uint16_t cid;
	uint16_t status;
}; /* 16 bytes */

/* Identify Namespace offsets */
#define NVME_ID_NS_NSZE_OFF  0x00 /* u64: total blocks */
#define NVME_ID_NS_LBAF_OFF  0x18 /* [3:0] selected LBA format index */
#define NVME_ID_NS_LBAFS_OFF 0x80 /* LBA format entries, 4 bytes each */
/* entry [23:16] = LBADS: LBA data size is 2^LBADS bytes */

#endif /* NVME_H_ */
