#ifndef _CAN_H
#define _CAN_H

#include <stdint.h>

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */
#define CANXL_PRIO_MASK CAN_SFF_MASK /* 11 bit priority mask */

/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28	: CAN identifier (11/29 bit)
 * bit 29	: error message frame flag (0 = data frame, 1 = error message)
 * bit 30	: remote transmission request flag (1 = rtr frame)
 * bit 31	: frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
typedef uint32_t canid_t;

struct can_frame {
    canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    uint8_t    can_dlc; /* frame payload length in byte (0 .. 8) */
    uint8_t    __pad;   /* padding */
    uint8_t    __res0;  /* reserved / padding */
    uint8_t    __res1;  /* reserved / padding */
    uint8_t    data[8] __attribute__((aligned(8)));
};

/* particular protocols of the protocol family PF_CAN */
#define CAN_RAW		1 /* RAW sockets */
#define CAN_BCM		2 /* Broadcast Manager */
#define CAN_TP16	3 /* VAG Transport Protocol v1.6 */
#define CAN_TP20	4 /* VAG Transport Protocol v2.0 */
#define CAN_MCNET	5 /* Bosch MCNet */
#define CAN_ISOTP	6 /* ISO 15765-2 Transport Protocol */
#define CAN_J1939	7 /* SAE J1939 */
#define CAN_NPROTO	8


#define SOL_CAN_BASE 100

/*
 * This typedef was introduced in Linux v3.1-rc2
 * (commit 6602a4b net: Make userland include of netlink.h more sane)
 * in <linux/socket.h>. It must be duplicated here to make the CAN
 * headers self-contained.
 */
typedef unsigned short __kernel_sa_family_t;

/**
 * struct sockaddr_can - the sockaddr structure for CAN sockets
 * @can_family:  address family number AF_CAN.
 * @can_ifindex: CAN network interface index.
 * @can_addr:    protocol specific address information
 */
struct sockaddr_can {
	__kernel_sa_family_t can_family;
	int         can_ifindex;
	union {
		/* transport protocol class address information (e.g. ISOTP) */
		struct { canid_t rx_id, tx_id; } tp;

		/* J1939 address information */
		struct {
			/* 8 byte name when using dynamic addressing */
			uint64_t name;

			/* pgn:
			 * 8 bit: PS in PDU2 case, else 0
			 * 8 bit: PF
			 * 1 bit: DP
			 * 1 bit: reserved
			 */
			uint32_t pgn;

			/* 1 byte address */
			uint8_t addr;
		} j1939;

		/* reserved for future CAN protocols address information */
	} can_addr;
};

#endif /* _CAN_H */
