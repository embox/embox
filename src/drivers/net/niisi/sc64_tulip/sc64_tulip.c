/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 16.01.25
 */

#include <assert.h>
#include <endian.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <asm/addrspace.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/mii.h>
#include <net/netdevice.h>
#include <net/phy.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>
#include <util/field.h>
#include <util/log.h>

#define TULIP_BASE_ADDR   OPTION_GET(NUMBER, base_addr)
#define TULIP_IRQ_NUM     OPTION_GET(NUMBER, irq_num)
#define TULIP_NUM_RX_DESC OPTION_GET(NUMBER, num_rx_desc)
#define TULIP_NUM_TX_DESC OPTION_GET(NUMBER, num_tx_desc)

static_assert(TULIP_NUM_RX_DESC >= 2, "Invalid num_rx_desc option!");
static_assert(TULIP_NUM_TX_DESC >= 2, "Invalid num_tx_desc option!");

/**
 * Control and Status Registers
 */
#define TULIP_CSR0  0x000 /* Bus Mode Register */
#define TULIP_CSR1  0x188 /* Transmit Poll Demand Register */
#define TULIP_CSR2  0x180 /* Receive Poll Demand Register */
#define TULIP_CSR3  0x190 /* Receive List Base Address Register */
#define TULIP_CSR4  0x198 /* Transmit List Base Address Register */
#define TULIP_CSR5  0x028 /* Status Register */
#define TULIP_CSR6  0x030 /* Operation Mode Register */
#define TULIP_CSR7  0x038 /* Interrupt Enable Register */
#define TULIP_CSR8  0x040 /* Missed Frames and Overflow Counter Register */
#define TULIP_CSR9  0x048 /* Boot ROM, Serial ROM and MII Management Register */
#define TULIP_CSR10 0x050 /* MDIO and Serial ROM Access Register */
#define TULIP_CSR11 0x058 /* Interrupt Mitigation Register */
#define TULIP_CSR12 0x060 /* Transmit Interrupt Mitigation Register */
#define TULIP_CSR13 0x068 /* Half-Duplex Data Flow Control Register */
#define TULIP_CSR14 0x070 /* Full-Duplex Data Flow Control Register */
#define TULIP_CSR15 0x078 /* General-Purpose Port and Watchdog Timer Register */

/**
 * Extended Status Registers
 */
#define TULIP_DEVID 0x04 /* (ESR0) Device ID */
#define TULIP_REVID 0x0c /* (ESR1) Revision ID */

/**
 * Accessing Register Bit Fields
 * 
 * (RO) - read only
 * (WO) - write only
 * (RW) - read/write
 * (RC) - read/clear
 */

/**
 * CSR0 Register Bit Fields
 */
#define TULIP_CSR0_RBUSAD (1 << 31) /* (RW) Receive Buffer Unavailable */
                                    /*      Special Action Disable */
#define TULIP_CSR0_CLRD   (1 << 24) /* (RW) Cache Line Read Disable */
#define TULIP_CSR0_DBO    (1 << 20) /* (RW) Descriptor Byte Ordering Mode */
#define TULIP_CSR0_M64    (1 << 16) /* (RW) 64-bit Mode */
#define TULIP_CSR0_BLE    (1 << 7)  /* (RW) Big/Little Endian */
#define TULIP_CSR0_BAD    (1 << 2)  /* (RW) Bus Arbitration Disable */
#define TULIP_CSR0_RSR    (1 << 1)  /* (WO) Receiver Software Reset */
#define TULIP_CSR0_TSR    (1 << 0)  /* (WO) Transmitter Software Reset */

#define TULIP_CSR0_RAP       /* (RW) Receive Automatic Polling */
#define TULIP_CSR0_RAP_MASK  0x7
#define TULIP_CSR0_RAP_SHIFT 28

#define TULIP_CSR0_TAP       /* (RW) Transmit Automatic Polling */
#define TULIP_CSR0_TAP_MASK  0x7
#define TULIP_CSR0_TAP_SHIFT 17

#define TULIP_CSR0_PBLR       /* (RW) Programmable Burst Length for Reading */
#define TULIP_CSR0_PBLR_MASK  0xf
#define TULIP_CSR0_PBLR_SHIFT 8

#define TULIP_CSR0_PBLW       /* (RW) Programmable Burst Length for Writing */
#define TULIP_CSR0_PBLW_MASK  0xf
#define TULIP_CSR0_PBLW_SHIFT 3

/**
 * CSR5 Register Bit Fields
 */
#define TULIP_CSR5_GPI (1 << 29) /* (RW) General Purpose Port Interrupt */
#define TULIP_CSR5_NIS (1 << 16) /* (RO) Normal Interrupt Summary */
#define TULIP_CSR5_AIS (1 << 15) /* (RO) Abnormal Interrupt Summary */
#define TULIP_CSR5_ERI (1 << 14) /* (RC) Early Receive Interrupt */
#define TULIP_CSR5_FTE (1 << 13) /* (RC) Fatal Transmitter Error */
#define TULIP_CSR5_FRE (1 << 12) /* (RC) Fatal Receiver Error */
#define TULIP_CSR5_ETI (1 << 10) /* (RC) Early Transmit Interrupt */
#define TULIP_CSR5_RWT (1 << 9)  /* (RC) Receive Watchdog Timeout */
#define TULIP_CSR5_RPS (1 << 8)  /* (RC) Receive Process Stopped */
#define TULIP_CSR5_RU  (1 << 7)  /* (RC) Receive Buffer Unavailable */
#define TULIP_CSR5_RI  (1 << 6)  /* (RC) Receive Interrupt */
#define TULIP_CSR5_UNF (1 << 5)  /* (RC) Transmit Underflow */
#define TULIP_CSR5_SBI (1 << 4)  /* (RO) Statistics Block Interrupt */
#define TULIP_CSR5_TJT (1 << 3)  /* (RC) Transmit Jabber Timeout */
#define TULIP_CSR5_TU  (1 << 2)  /* (RC) Transmit Buffer Unavailable */
#define TULIP_CSR5_TPS (1 << 1)  /* (RC) Transmit Process Stopped */
#define TULIP_CSR5_TI  (1 << 0)  /* (RC) Transmit Interrupt */

#define TULIP_CSR5_ET       /* (RO) Transmitter Error Bits */
#define TULIP_CSR5_ET_MASK  0x7
#define TULIP_CSR5_ET_SHIFT 26

#define TULIP_CSR5_ER       /* (RO) Receiver Error Bits */
#define TULIP_CSR5_ER_MASK  0x7
#define TULIP_CSR5_ER_SHIFT 23

#define TULIP_CSR5_TS       /* (RO) Transmit Process State */
#define TULIP_CSR5_TS_MASK  0x7
#define TULIP_CSR5_TS_SHIFT 20

#define TULIP_CSR5_RS       /* (RO) Receive Process State */
#define TULIP_CSR5_RS_MASK  0x7
#define TULIP_CSR5_RS_SHIFT 17

/**
 * CSR6 Register Bit Fields
 */
#define TULIP_CSR6_RA  (1 << 30) /* (RW) Receive All */
#define TULIP_CSR6_TTM (1 << 22) /* (RW) Transmit Threshold Mode */
#define TULIP_CSR6_RFS (1 << 20) /* (RW) Receive FCS Skip */
#define TULIP_CSR6_EDE (1 << 19) /* (RW) Ethernet Decapsulation Enable */
#define TULIP_CSR6_VD  (1 << 18) /* (RW) VLAN Disable */
#define TULIP_CSR6_CA  (1 << 17) /* (RW) Capture Effect Enable */
#define TULIP_CSR6_ST  (1 << 13) /* (RW) Start/Stop Transmission */
#define TULIP_CSR6_FC  (1 << 12) /* (RW) Force Collision */
#define TULIP_CSR6_FD  (1 << 9)  /* (RW) Full Duplex */
#define TULIP_CSR6_PM  (1 << 7)  /* (RW) Pass All Multicast */
#define TULIP_CSR6_PR  (1 << 6)  /* (RW) Promiscuous Mode */
#define TULIP_CSR6_IF  (1 << 4)  /* (RW) Inverse Filtering Mode */
#define TULIP_CSR6_PB  (1 << 3)  /* (RW) Pass Bad Frames */
#define TULIP_CSR6_HO  (1 << 2)  /* (RW) Hash-Only Filtering Mode */
#define TULIP_CSR6_SR  (1 << 1)  /* (RW) Start/Stop Receive */
#define TULIP_CSR6_HP  (1 << 0)  /* (RW) Hash/Perfect Receive Filtering Mode */

#define TULIP_CSR6_OM       /* (RW) Operating Mode */
#define TULIP_CSR6_OM_MASK  0x3
#define TULIP_CSR6_OM_SHIFT 10

/**
 * CSR7 Register Bit Fields
 */
#define TULIP_CSR7_GPE (1 << 29) /* (RW) General Purpose Port Interrupt */
#define TULIP_CSR7_NIE (1 << 16) /* (RW) Normal Interrupt Summary Enable */
#define TULIP_CSR7_AIE (1 << 15) /* (RW) Abnormal Interrupt Summary Enable */
#define TULIP_CSR7_ERE (1 << 14) /* (RW) Early RX Interrupt Enable */
#define TULIP_CSR7_TEN (1 << 13) /* (RW) Fatal TX Error Interrupt Enable */
#define TULIP_CSR7_REN (1 << 12) /* (RW) Fatal RX Error Interrupt Enable */
#define TULIP_CSR7_ETE (1 << 10) /* (RW) Early TX Interrupt Enable */
#define TULIP_CSR7_RWE (1 << 9)  /* (RW) RX Watchdog Timeout Interrupt Enable */
#define TULIP_CSR7_RSE (1 << 8)  /* (RW) RX Process Stopped Interrupt Enable */
#define TULIP_CSR7_RUE (1 << 7)  /* (RW) RX Buff Unavailable Interrupt Enable */
#define TULIP_CSR7_RIE (1 << 6)  /* (RW) RX Interrupt Enable */
#define TULIP_CSR7_UNE (1 << 5)  /* (RW) TX Underflow Interrupt Enable */
#define TULIP_CSR7_THE (1 << 3)  /* (RW) TX Jabber Timeout Interrupt Enable */
#define TULIP_CSR7_TUE (1 << 2)  /* (RW) TX Buff Unavailable Interrupt Enable */
#define TULIP_CSR7_TSE (1 << 1)  /* (RW) TX Process Stopped Interrupt Enable */
#define TULIP_CSR7_TIE (1 << 0)  /* (RW) TX Interrupt Enable */

/**
 * CSR9 Register Bit Fields
 */
#define TULIP_CSR9_MDI  (1 << 19) /* (RO) MDIO Data In */
#define TULIP_CSR9_MDOM (1 << 18) /* (RW) MDIO Operation Mode (Read/Write) */
#define TULIP_CSR9_MDO  (1 << 17) /* (RW) MDIO Data Out */
#define TULIP_CSR9_MDC  (1 << 16) /* (RW) MDIO Clock */

/**
 * CSR14 Register Bit Fields
 */
#define TULIP_CSR14_PSPE (1 << 1) /* (RW) Pause Stop Enable */
#define TULIP_CSR14_PSDE (1 << 0) /* (RW) Pause Send Enable */

/**
 * Macros for accessing CSR registers
 */
#define TULIP_CSR_STORE(csr, val) \
	REG32_STORE(TULIP_BASE_ADDR + csr, htole32(val))

#define TULIP_CSR_LOAD(csr) le32toh(REG32_LOAD(TULIP_BASE_ADDR + csr))

#define TULIP_CSR_ORIN(csr, val) REG32_ORIN(TULIP_BASE_ADDR + csr, htole32(val))

#define TULIP_CSR_ANDIN(csr, val) \
	REG32_ANDIN(TULIP_BASE_ADDR + csr, htole32(val))

#define TULIP_CSR_CLEAR(csr, val) \
	REG32_CLEAR(TULIP_BASE_ADDR + csr, htole32(val))

/**
 * Receiver Descriptor Bit Fields
 */
#define TULIP_RDES0_OWN (1 << 31) /* Own Bit */
#define TULIP_RDES0_ES  (1 << 15) /* Error Summary */
#define TULIP_RDES0_DE  (1 << 14) /* Descriptor Error */
#define TULIP_RDES0_RF  (1 << 11) /* Runt Frame */
#define TULIP_RDES0_FS  (1 << 9)  /* First Descriptor */
#define TULIP_RDES0_LS  (1 << 8)  /* Last Descriptor */
#define TULIP_RDES0_FF  (1 << 5)  /* Filtering Fail */
#define TULIP_RDES0_RW  (1 << 4)  /* Receive Watchdog */
#define TULIP_RDES0_RE  (1 << 3)  /* Report On MII Error */
#define TULIP_RDES0_DB  (1 << 2)  /* Dribbling Bit */
#define TULIP_RDES0_CE  (1 << 1)  /* CRC Error */
#define TULIP_RDES0_FO  (1 << 0)  /* FIFO Overflow */

#define TULIP_RDES0_PFL       /* Part of Frame Length */
#define TULIP_RDES0_PFL_MASK  0x7fff
#define TULIP_RDES0_PFL_SHIFT 16

#define TULIP_RDES0_MF           /* Multicast Frame */
#define TULIP_RDES0_MF_MASK      0x3
#define TULIP_RDES0_MF_SHIFT     6
#define TULIP_RDES0_MF_PHYSICAL  0b01
#define TULIP_RDES0_MF_MULTICAST 0b10
#define TULIP_RDES0_MF_BROADCAST 0b11

#define TULIP_RDES1_FT   (1 << 3) /* Frame Type */
#define TULIP_RDES1_VLAN (1 << 2) /* VLAN Packet */
#define TULIP_RDES1_LO   (1 << 0) /* Length Out */

#define TULIP_RDES1_FL       /* Frame Length */
#define TULIP_RDES1_FL_MASK  0xffff
#define TULIP_RDES1_FL_SHIFT 16

#define TULIP_RDES4_CS       /* Checksum */
#define TULIP_RDES4_CS_MASK  0xffff
#define TULIP_RDES4_CS_SHIFT 16

#define TULIP_RDES5_RBS       /* RX Buffer Size */
#define TULIP_RDES5_RBS_MASK  0x7fff
#define TULIP_RDES5_RBS_SHIFT 0

/**
 * Transmitter Descriptor Bit Fields
 */
#define TULIP_TDES0_OWN (1 << 31) /* Own Bit */
#define TULIP_TDES0_ES  (1 << 15) /* Error Summary */
#define TULIP_TDES0_TO  (1 << 14) /* Transmit Jabber Timeout */
#define TULIP_TDES0_CF  (1 << 11) /* Carrier Failure */
#define TULIP_TDES0_NCS (1 << 10) /* No Checksum transmitted */
#define TULIP_TDES0_LC  (1 << 9)  /* Late Collision */
#define TULIP_TDES0_EC  (1 << 8)  /* Excessive Collisions */
#define TULIP_TDES0_WTL (1 << 7)  /* Wrong Type/Length */
#define TULIP_TDES0_UF  (1 << 1)  /* Underflow Error */
#define TULIP_TDES0_DE  (1 << 0)  /* Excessive Defering */

#define TULIP_TDES0_BC       /* Transmitted Byte Count */
#define TULIP_TDES0_BC_MASK  0x7fff
#define TULIP_TDES0_BC_SHIFT 16

#define TULIP_TDES3_PC       /* Pseudo-header Checksum */
#define TULIP_TDES3_PC_MASK  0xffff
#define TULIP_TDES3_PC_SHIFT 16

#define TULIP_TDES4_TCP (1 << 29) /* TCP/UDP Packet */
#define TULIP_TDES4_CE  (1 << 28) /* Checksum Calculation Enable */

#define TULIP_TDES4_CI       /* Checksum Insert Byte Number */
#define TULIP_TDES4_CI_MASK  0x3ff
#define TULIP_TDES4_CI_SHIFT 16

#define TULIP_TDES4_CS       /* Checksum Start Byte Number */
#define TULIP_TDES4_CS_MASK  0x3ff
#define TULIP_TDES4_CS_SHIFT 0

#define TULIP_TDES5_IC  (1 << 31) /* Interrupt on Completion */
#define TULIP_TDES5_LS  (1 << 30) /* Last Segment */
#define TULIP_TDES5_FS  (1 << 29) /* First Segment */
#define TULIP_TDES5_FT1 (1 << 28) /* Filtering Type */
#define TULIP_TDES5_SET (1 << 27) /* Setup Packet */
#define TULIP_TDES5_AC  (1 << 26) /* Add CRC Disable */
#define TULIP_TDES5_FT0 (1 << 25) /* Filtering Type */

#define TULIP_TDES5_TBS       /* TX Buffer Size */
#define TULIP_TDES5_TBS_MASK  0x3fff
#define TULIP_TDES5_TBS_SHIFT 0

struct tulip_rx_desc {
	uint32_t rdes0; /* Status */
	uint32_t rdes1; /* Auxiliary Status */
	uint32_t rdes2; /* RX Time Stamp (LSB) */
	uint32_t rdes3; /* RX Time Stamp (MSB) */
	uint32_t rdes4; /* Checksum */
	uint32_t rdes5; /* Buffer Size */
	uint32_t rdes6; /* Next Descriptor Address (LSB) */
	uint32_t rdes7; /* Next Descriptor Address (MSB) */
	uint32_t rdes8; /* Buffer Address (LSB) */
	uint32_t rdes9; /* Buffer Address (MSB) */
};

struct tulip_tx_desc {
	uint32_t tdes0; /* Status */
	uint32_t tdes1; /* TX Time Stamp (LSB) */
	uint32_t tdes2; /* TX Time Stamp (MSB) */
	uint32_t tdes3; /* Header Checksum */
	uint32_t tdes4; /* Checksum */
	uint32_t tdes5; /* Buffer Size */
	uint32_t tdes6; /* Next Descriptor Address (LSB) */
	uint32_t tdes7; /* Next Descriptor Address (MSB) */
	uint32_t tdes8; /* Buffer Address (LSB) */
	uint32_t tdes9; /* Buffer Address (MSB) */
};

#define TULIP_FRAME_LEN 1518

static uint8_t rx_buff[TULIP_NUM_RX_DESC][TULIP_FRAME_LEN]
    __attribute__((aligned(8)));
static uint8_t tx_buff[TULIP_NUM_TX_DESC][TULIP_FRAME_LEN]
    __attribute__((aligned(8)));

static struct tulip_rx_desc rx_ring[TULIP_NUM_RX_DESC]
    __attribute__((aligned(8)));
static struct tulip_tx_desc tx_ring[TULIP_NUM_TX_DESC]
    __attribute__((aligned(8)));

static unsigned int rx_curr;
static unsigned int tx_curr;
static unsigned int phy_id;

/**
 * Macros for Accessing Descriptors
 */
#define TULIP_DESC_STORE(desc, val) REG32_STORE(desc, htole32(val))
#define TULIP_DESC_LOAD(desc)       le32toh(REG32_LOAD(desc))
#define TULIP_DESC_ORIN(desc, val)  REG32_ORIN(desc, htole32(val))
#define TULIP_DESC_ANDIN(desc, val) REG32_ANDIN(desc, htole32(val))
#define TULIP_DESC_CLEAR(desc, val) REG32_CLEAR(desc, htole32(val))

static void tulip_set_phyid(struct net_device *dev, uint8_t phyid) {
	if (phyid > 0x1f) {
		log_error("Invalid phy id");
	}
	else {
		phy_id = phyid;
	}
}

#define TULIP_MDIO_DELAY() (void)TULIP_CSR_LOAD(TULIP_CSR9)

static int tulip_mdio_read(struct net_device *dev, uint8_t reg) {
	uint32_t data;
	uint32_t cmd;
	uint32_t val;
	int i;

	val = 0;
	cmd = (0xf6 << 10) | (phy_id << 5) | reg;

	/* Establish sync by sending at least 32 logic ones */
	for (i = 32; i >= 0; i--) {
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDO);
		TULIP_MDIO_DELAY();
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDO | TULIP_CSR9_MDC);
		TULIP_MDIO_DELAY();
	}

	/* Shift the read command bits out */
	for (i = 15; i >= 0; i--) {
		data = (cmd & (1 << i)) ? TULIP_CSR9_MDO : 0;

		TULIP_CSR_STORE(TULIP_CSR9, data);
		TULIP_MDIO_DELAY();
		TULIP_CSR_STORE(TULIP_CSR9, data | TULIP_CSR9_MDC);
		TULIP_MDIO_DELAY();
	}

	/* Read the two transition, 16 data, and wire-idle bits */
	for (i = 19; i > 0; i--) {
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDOM);
		TULIP_MDIO_DELAY();
		data = TULIP_CSR_LOAD(TULIP_CSR9);
		val = (val << 1) | ((data & TULIP_CSR9_MDI) ? 1 : 0);
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDOM | TULIP_CSR9_MDC);
		TULIP_MDIO_DELAY();
	}

	val = (val >> 1) & 0xffff;

	return val;
}

static int tulip_mdio_write(struct net_device *dev, uint8_t reg, uint16_t val) {
	uint32_t data;
	uint32_t cmd;
	int i;

	cmd = (0x5002 << 16) | (phy_id << 23) | (reg << 18) | val;

	/* Establish sync by sending 32 logic ones */
	for (i = 32; i >= 0; i--) {
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDO);
		TULIP_MDIO_DELAY();
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDO | TULIP_CSR9_MDC);
		TULIP_MDIO_DELAY();
	}

	/* Shift the command bits out */
	for (i = 31; i >= 0; i--) {
		data = (cmd & (1 << i)) ? TULIP_CSR9_MDO : 0;

		TULIP_CSR_STORE(TULIP_CSR9, data);
		TULIP_MDIO_DELAY();
		TULIP_CSR_STORE(TULIP_CSR9, data | TULIP_CSR9_MDC);
		TULIP_MDIO_DELAY();
	}

	/* Clear out extra bits */
	for (i = 2; i > 0; i--) {
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDOM);
		TULIP_MDIO_DELAY();
		TULIP_CSR_STORE(TULIP_CSR9, TULIP_CSR9_MDOM | TULIP_CSR9_MDC);
		TULIP_MDIO_DELAY();
	}

	return 0;
}

static int tulip_set_speed(struct net_device *dev, int speed) {
	uint32_t csr6_old;
	uint32_t csr6_new;

	csr6_new = csr6_old = TULIP_CSR_LOAD(TULIP_CSR6);

	if (net_is_fullduplex(speed)) {
		csr6_new |= TULIP_CSR6_FD;
	}
	else {
		csr6_new &= ~TULIP_CSR6_FD;
	}

	speed = net_to_mbps(speed);

	switch (speed) {
	case 10:
		csr6_new |= TULIP_CSR6_TTM;
		break;
	case 100:
		csr6_new &= ~TULIP_CSR6_TTM;
		break;
	case 1000:
		log_error("Gigabit mode not supported");
		break;
	default:
		log_error("Invalid Ethernet speed");
		break;
	}

	if (csr6_new != csr6_old) {
		TULIP_CSR_STORE(TULIP_CSR6, csr6_new);
	}

	return 0;
}

#define SETUP_FRAME_LEN 192

static int tulip_set_macaddr(struct net_device *dev, const void *addr) {
	struct tulip_tx_desc *desc;
	const uint8_t *macaddr;

	uint8_t *buff;
	int i;

	desc = &tx_ring[tx_curr];
	buff = (void *)KSEG1ADDR(tx_buff[tx_curr]);
	macaddr = addr;

	memset(buff, 0xff, SETUP_FRAME_LEN);

	buff[0] = macaddr[0];
	buff[1] = macaddr[1];
	buff[4] = macaddr[2];
	buff[5] = macaddr[3];
	buff[8] = macaddr[4];
	buff[9] = macaddr[5];

	TULIP_DESC_STORE(&desc->tdes5, SETUP_FRAME_LEN | TULIP_TDES5_SET);
	TULIP_DESC_STORE(&desc->tdes0, TULIP_TDES0_OWN);
	TULIP_CSR_STORE(TULIP_CSR1, 1);

	for (i = 0; TULIP_DESC_LOAD(&desc->tdes0) & TULIP_TDES0_OWN; i++) {
		if (i == 10) {
			log_error("Cannot process setup packet");
			return -ENXIO;
		}
		usleep(1000);
	}

	tx_curr = (tx_curr + 1) % TULIP_NUM_TX_DESC;

	return 0;
}

static int tulip_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct tulip_tx_desc *desc;
	uint8_t *buff;
	void *data;
	int i;

	desc = &tx_ring[tx_curr];
	buff = (void *)KSEG1ADDR(tx_buff[tx_curr]);
	data = skb_data_cast_in(skb->data);

	for (i = 0; TULIP_DESC_LOAD(&desc->tdes0) & TULIP_TDES0_OWN; i++) {
		if (i == 10) {
			log_warning("TX buffer not ready");
			return -EBUSY;
		}
		usleep(1000);
	}

	memcpy(buff, data, skb->len);

	TULIP_DESC_STORE(&desc->tdes5, skb->len | TULIP_TDES5_FS | TULIP_TDES5_LS);
	TULIP_DESC_STORE(&desc->tdes0, TULIP_TDES0_OWN);
	TULIP_CSR_STORE(TULIP_CSR1, 1);

	tx_curr = (tx_curr + 1) % TULIP_NUM_TX_DESC;

	show_packet(data, skb->len, "transmitted");

	skb_free(skb);

	return 0;
}

static irq_return_t tulip_irq_handler(unsigned int irq_num, void *_dev) {
	struct tulip_rx_desc *desc;
	struct sk_buff *skb;
	uint32_t status;
	uint8_t *buff;
	void *data;

	for (;;) {
		desc = &rx_ring[rx_curr];
		buff = (void *)KSEG1ADDR(rx_buff[rx_curr]);
		status = TULIP_DESC_LOAD(&desc->rdes0);

		if (status & TULIP_RDES0_OWN) {
			break;
		}

		/* Clear interrupt */
		TULIP_CSR_STORE(TULIP_CSR5, TULIP_CSR5_RI);

		if (!(status & TULIP_RDES0_LS)) {
			log_error("RX error: packet too big\n", status);
		}

		if (status & TULIP_RDES0_ES) {
			log_error("RX error: status=0x%x\n", status);
		}

		skb = skb_alloc(ETH_FRAME_LEN);
		assert(skb);

		skb->dev = _dev;
		skb->len = FIELD_GET(status, TULIP_RDES0_PFL) - 4;

		data = skb_data_cast_in(skb->data);
		memcpy(data, buff, skb->len);

		show_packet(data, skb->len, "received");

		/* Pass packet on to network subsystem */
		netif_rx(skb);

		/* Reenable descriptor */
		TULIP_DESC_STORE(&desc->rdes0, TULIP_RDES0_OWN);
		TULIP_CSR_STORE(TULIP_CSR2, 1);

		rx_curr = (rx_curr + 1) % TULIP_NUM_RX_DESC;
	}

	return IRQ_HANDLED;
}

static int tulip_reset(void) {
	uint32_t csr5;
	int i;

	TULIP_CSR_ORIN(TULIP_CSR0, TULIP_CSR0_TSR | TULIP_CSR0_RSR);
	usleep(1000);
	TULIP_CSR_STORE(TULIP_CSR0, 0);
	usleep(1000);

	for (i = 0; i < 10; i++) {
		csr5 = TULIP_CSR_LOAD(TULIP_CSR5);
		if ((FIELD_GET(csr5, TULIP_CSR5_TS) == 0)
		    && (FIELD_GET(csr5, TULIP_CSR5_RS) == 0)) {
			return 0;
		}
		usleep(10000);
	}

	log_error("Cannot reset ethernet controller");

	return -ENXIO;
}

static int tulip_start(struct net_device *dev) {
	int err;
	int i;

	rx_curr = 0;
	tx_curr = 0;

	memset((void *)KSEG1ADDR(rx_ring), 0, sizeof(rx_ring));
	memset((void *)KSEG1ADDR(tx_ring), 0, sizeof(tx_ring));

	err = tulip_reset();
	if (err) {
		return err;
	}

#if 0
	err = phy_init(dev);
	if (err) {
		return err;
	}
#else
	TULIP_CSR_ORIN(TULIP_CSR6, TULIP_CSR6_FD);
	TULIP_CSR_CLEAR(TULIP_CSR6, TULIP_CSR6_TTM);
#endif

	err = irq_attach(TULIP_IRQ_NUM, tulip_irq_handler, 0, dev, "sc64-tulip");
	if (err) {
		return err;
	}

	for (i = 0; i < TULIP_NUM_RX_DESC; i++) {
		TULIP_DESC_STORE(&rx_ring[i].rdes5, TULIP_FRAME_LEN);
		TULIP_DESC_STORE(&rx_ring[i].rdes6,
		    PHYSADDR(&rx_ring[(i + 1) % TULIP_NUM_RX_DESC]));
		TULIP_DESC_STORE(&rx_ring[i].rdes8, PHYSADDR(rx_buff[i]));
		TULIP_DESC_STORE(&rx_ring[i].rdes0, TULIP_RDES0_OWN);
	}

	for (i = 0; i < TULIP_NUM_TX_DESC; i++) {
		TULIP_DESC_STORE(&tx_ring[i].tdes6,
		    PHYSADDR(&tx_ring[(i + 1) % TULIP_NUM_TX_DESC]));
		TULIP_DESC_STORE(&tx_ring[i].tdes8, PHYSADDR(tx_buff[i]));
	}

	TULIP_CSR_STORE(TULIP_CSR3, PHYSADDR(rx_ring));
	TULIP_CSR_STORE(TULIP_CSR4, PHYSADDR(tx_ring));

	TULIP_CSR_STORE(TULIP_CSR7, TULIP_CSR7_RIE | TULIP_CSR7_NIE);
	TULIP_CSR_ORIN(TULIP_CSR6, TULIP_CSR6_PM | TULIP_CSR6_SR | TULIP_CSR6_ST);

	return 0;
}

static int tulip_stop(struct net_device *dev) {
	TULIP_CSR_STORE(TULIP_CSR7, 0);
	TULIP_CSR_CLEAR(TULIP_CSR6, TULIP_CSR6_SR | TULIP_CSR6_ST);

	irq_detach(TULIP_IRQ_NUM, dev);

	return 0;
}

static const struct net_driver tulip_ops = {
    .start = tulip_start,
    .stop = tulip_stop,
    .xmit = tulip_xmit,
    .mdio_read = tulip_mdio_read,
    .mdio_write = tulip_mdio_write,
    .set_phyid = tulip_set_phyid,
    .set_macaddr = tulip_set_macaddr,
    .set_speed = tulip_set_speed,
};

EMBOX_UNIT_INIT(tulip_init);

static int tulip_init(void) {
	struct net_device *dev;
	int err;

	/* Allocate ethernet device */
	dev = etherdev_alloc(0);
	if (dev == NULL) {
		return -ENOMEM;
	}

	/* Init ethernet device */
	dev->drv_ops = &tulip_ops;
	dev->base_addr = TULIP_BASE_ADDR;
	dev->irq = TULIP_IRQ_NUM;

	err = inetdev_register_dev(dev);
	if (err) {
		return err;
	}

	return 0;
}
