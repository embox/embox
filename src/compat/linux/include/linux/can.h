/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#ifndef COMPAT_LINUX_LINUX_CAN_H_
#define COMPAT_LINUX_LINUX_CAN_H_

#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>

/* Special bits of the 'can_id' member of struct can_frame */
#define CAN_EFF_FLAG 0x80000000 /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000 /* Remote transmission request */
#define CAN_ERR_FLAG 0x20000000 /* Error message frame */

/* ID bits of the 'can_id' member of struct can_frame */
#define CAN_SFF_MASK 0x000007ff /* Standard frame format (SFF) */
#define CAN_EFF_MASK 0x1fffffff /* Extended frame format (EFF) */
#define CAN_ERR_MASK 0x1fffffff /* Omit EFF, RTR, ERR flags */

/* The number of ID bits in 'can_id' member of struct can_frame */
#define CAN_SFF_ID_BITS 11
#define CAN_EFF_ID_BITS 29

/* Maximum Transfer Unit  */
#define CAN_MTU   (sizeof(struct can_frame))
#define CANFD_MTU (sizeof(struct canfd_frame))

/* CAN payload length and DLC definitions according to ISO 11898-1 */
#define CAN_MAX_DLC  8
#define CAN_MAX_DLEN 8

/* CAN FD payload length and DLC definitions according to ISO 11898-7 */
#define CANFD_MAX_DLC  15
#define CANFD_MAX_DLEN 64

/* Bits of the 'flags' member of struct canfd_frame */
#define CANFD_BRS 0x01 /* Bit rate switch (second bitrate for payload data) */
#define CANFD_ESI 0x02 /* Error state indicator of the transmitting node */
#define CANFD_FDF 0x04 /* Mark CAN FD for dual use of struct canfd_frame */

/* Bits of the 'can_id' member of struct can_filter */
#define CAN_INV_FILTER 0x20000000U /* To be set in can_filter.can_id */

/* Values of the 'protocol' arg of socket() */
#define CAN_RAW    1 /* RAW sockets */
#define CAN_BCM    2 /* Broadcast Manager */
#define CAN_TP16   3 /* VAG Transport Protocol v1.6 */
#define CAN_TP20   4 /* VAG Transport Protocol v2.0 */
#define CAN_MCNET  5 /* Bosch MCNet */
#define CAN_ISOTP  6 /* ISO 15765-2 Transport Protocol */
#define CAN_J1939  7 /* SAE J1939 */
#define CAN_NPROTO 8

#define SOL_CAN_BASE 100

/* Values of the 'level' arg of setsockopt() */
#define SOL_CAN_RAW (SOL_CAN_BASE + CAN_RAW)

/* Values of the 'optname' arg of setsockopt() */
#define CAN_RAW_FILTER        32
#define CAN_RAW_ERR_FILTER    33
#define CAN_RAW_LOOPBACK      34
#define CAN_RAW_RECV_OWN_MSGS 35
#define CAN_RAW_FD_FRAMES     36
#define CAN_RAW_JOIN_FILTERS  37
#define CAN_RAW_TX_DEADLINE   38

typedef uint32_t canid_t;
typedef uint32_t can_err_mask_t;

/* Basic CAN frame */
struct can_frame {
	canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	uint8_t can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
	uint8_t __pad;   /* padding */
	uint8_t __res0;  /* reserved / padding */
	uint8_t __res1;  /* reserved / padding */
	uint8_t data[CAN_MAX_DLEN];
};

/* CAN flexible data rate frame */
struct canfd_frame {
	canid_t can_id; /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	uint8_t len;    /* frame payload length in byte */
	uint8_t flags;  /* additional flags for CAN FD */
	uint8_t __res0; /* reserved / padding */
	uint8_t __res1; /* reserved / padding */
	uint8_t data[CANFD_MAX_DLEN];
};

/* Structure used to bind a socket to a CAN network interface */
struct sockaddr_can {
	sa_family_t can_family; /* AF_CAN */
	int can_ifindex;        /* CAN Interface Index */
	union {
		/* Transport protocol specific address information */
		struct {
			canid_t rx_id, tx_id;
		} tp;
		struct {
			uint64_t name; /* 64-bit Device NAME (for dynamic addressing) */
			uint32_t pgn;  /* Parameter Group Number */
			uint8_t addr;  /* 8-bit Source/Destination Address */
		} j1939;
	} can_addr;
};

/* Structure used as 'optval' arg in setsockopt() with CAN_RAW_FILTER */
struct can_filter {
	canid_t can_id;   /* relevant bits of CAN ID */
	canid_t can_mask; /* mask for the can_id */
};

#endif /* COMPAT_LINUX_LINUX_CAN_H_ */
