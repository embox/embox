/**
 * @file
 * @brief Ethernet Media Access Controller for TMS320DM816x DaVinci
 *
 * @date 20.09.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_ETHERNET_TI816X_H_
#define DRIVERS_ETHERNET_TI816X_H_

/**
 * EMAC0/MDIO Base Address
 */
#define EMAC_BASE_ADDR 0x4A100000

/**
 * CPGMAC0 Interrupts
 */
#define MACRXTHR0 40 /* CPGMAC0 Receive threshold interrupt */
#define MACRXINT0 41 /* CPGMAC0 Receive pending interrupt */
#define MACTXINT0 42 /* CPGMAC0 Transmit pending interrupt */
#define MACMISC0  43 /* CPGMAC0 Stat, Host, MDIO LINKINT or MDIO USERINT */

/**
 * EMAC/MDIO Registers - Base Address Offset
 */
#define EMAC_OFFSET      0x00000000 /* EMAC module */
#define EMAC_CTRL_OFFSET 0x00000900 /* EMAC control module */

/**
 * EMAC0/MDIO Module and Control Module Base Address
 */
#define EMAC_BASE \
	(EMAC_BASE_ADDR + EMAC_OFFSET)      /* EMAC Module Base */
#define EMAC_CTRL_BASE \
	(EMAC_BASE_ADDR + EMAC_CTRL_OFFSET) /* EMAC Control Module Base */

#define EMAC_CHANNEL_COUNT 8

/**
 * EMAC Control Module Registers
 */
#define EMAC_R_CMIDVER           0x00 /* Identification and Version Register */
#define EMAC_R_CMSOFTRESET       0x04 /* Software Reset Register */
#define EMAC_R_CMEMCONTROL       0x08 /* Emulation Control Register */
#define EMAC_R_CMINTCTRL         0x0C /* Interrupt Control Register */
#define EMAC_R_CMRXTHRESHINTEN   0x10 /* Receive Threshold Interrupt Enable
										 Register */
#define EMAC_R_CMRXINTEN         0x14 /* Receive Interrupt Enable Register */
#define EMAC_R_CMTXINTEN         0x18 /* Transmit Interrupt Enable Register */
#define EMAC_R_CMMISCINTEN       0x1C /* Miscellaneous Interrupt Enable
										 Register */
#define EMAC_R_CMRXTHRESHINTSTAT 0x40 /* Receive Threshold Interrupt Status
										 Register */
#define EMAC_R_CMRXINTSTAT       0x44 /* Receive Interrupt Status Register */
#define EMAC_R_CMTXINTSTAT       0x48 /* Transmit Interrupt Status Register */
#define EMAC_R_CMMISCINTSTAT     0x4C /* Miscellaneous Interrupt Status
										 Register */
#define EMAC_R_CMRXINTMAX        0x70 /* Receive Interrupts Per Millisecond
										 Register */
#define EMAC_R_CMTXINTMAX        0x74 /* Transmit Interrupts Per Millisecond
										 Register */

/**
 * EMAC Module Registers
 */
#define EMAC_R_CPGMACIDVER       0x000 /* Identification and Version Register */
#define EMAC_R_TXCONTROL         0x004 /* Transmit Control Register */
#define EMAC_R_TXTEARDOWN        0x008 /* Transmit Teardown Register */
#define EMAC_R_RXCONTROL         0x014 /* Receive Control Register */
#define EMAC_R_RXTEARDOWN        0x018 /* Receive Teardown Register */
#define EMAC_R_TXINTSTATRAW      0x080 /* Transmit Interrupt Status (Unmasked)
										  Register */
#define EMAC_R_TXINTSTATMASKED   0x084 /* Transmit Interrupt Status (Masked)
										  Register */
#define EMAC_R_TXINTMASKSET      0x088 /* Transmit Interrupt Mask Set
										  Register */
#define EMAC_R_TXINTMASKCLEAR    0x08C /* Transmit Interrupt Clear Register */
#define EMAC_R_MACINVECTOR       0x090 /* MAC Input Vector Registe */
#define EMAC_R_MACEOIVECTOR      0x094 /* MAC End of Interrupt Vector
										  Register */
#define EMAC_R_RXINTSTATRAW      0x0A0 /* Receive Interrupt Status (Unmasked)
										  Register */
#define EMAC_R_RXINTSTATMASKED   0x0A4 /* Receive Interrupt Status (Masked)
										  Register */
#define EMAC_R_RXINTMASKSET      0x0A8 /* Receive Interrupt Mask Set Register */
#define EMAC_R_RXINTMASKCLEAR    0x0AC /* Receive Interrupt Mask Clear
										  Register */
#define EMAC_R_MACINTSTATRAW     0x0B0 /* MAC Interrupt Status (Unmasked)
										  Register */
#define EMAC_R_MACINTSTATMASKED  0x0B4 /* MAC Interrupt Status (Masked)
										  Register */
#define EMAC_R_MACINTMASKSET     0x0B8 /* MAC Interrupt Mask Set Register */
#define EMAC_R_MACINTMASKCLEAR   0x0BC /* MAC Interrupt Mask Clear Register */
#define EMAC_R_RXMBPENABLE       0x100 /* Receive Multicast/Broadcast/
										  Promiscuous Channel Enable Register */
#define EMAC_R_RXUNICASTSET      0x104 /* Receive Unicast Enable Set Register */
#define EMAC_R_RXUNICASTCLEAR    0x108 /* Receive Unicast Clear Register */
#define EMAC_R_RXMAXLEN          0x10C /* Receive Maximum Length Register */
#define EMAC_R_RXBUFFEROFFSET    0x110 /* Receive Buffer Offset Register */
#define EMAC_R_RXFILTERLOWTHRESH 0x114 /* Receive Filter Low Priority Frame
										  Threshold Register */
#define EMAC_R_RXFLOWTHRESH(n)  (0x120 + n * 0x4) /* Receive Channel n[0-7]
													 Flow Control Threshold
													 Register */
#define EMAC_R_RXFREEBUFFER(n)  (0x140 + n * 0x4) /* Receive Channel n[0-7]
													 Free Buffer Count
													 Register */
#define EMAC_R_MACCONTROL        0x160 /* MAC Control Register */
#define EMAC_R_MACSTATUS         0x164 /* MAC Status Register */
#define EMAC_R_EMCONTROL         0x168 /* Emulation Control Register */
#define EMAC_R_FIFOCONTROL       0x16C /* FIFO Control Register */
#define EMAC_R_MACCONFIG         0x170 /* MAC Configuration Register */
#define EMAC_R_SOFTRESET         0x174 /* Soft Reset Register */
#define EMAC_R_MACSRCADDRLO      0x1D0 /* MAC Source Address Low Bytes
										  Register */
#define EMAC_R_MACSRCADDRHI      0x1D4 /* MAC Source Address High Bytes
										  Register */
#define EMAC_R_MACHASH1          0x1D8 /* MAC Hash Address Register 1 */
#define EMAC_R_MACHASH2          0x1DC /* MAC Hash Address Register 2 */
#define EMAC_R_BOFFTEST          0x1E0 /* Back Off Test Register */
#define EMAC_R_TPACETEST         0x1E4 /* Transmit Pacing Algorithm Test
										  Register */
#define EMAC_R_RXPAUSE           0x1E8 /* Receive Pause Timer Register */
#define EMAC_R_TXPAUSE           0x1EC /* Transmit Pause Timer Register */
#define EMAC_R_MACADDRLO         0x500 /* MAC Address Low Bytes Register,
										  Used in Receive Address Matching */
#define EMAC_R_MACADDRHI         0x504 /* MAC Address High Bytes Register,
										  Used in Receive Address Matching */
#define EMAC_R_MACINDEX          0x508 /* MAC Index Register */
#define EMAC_R_TXHDP(n)          0x600 /* Transmit Channel n[0-7] DMA Head
										  Descriptor Pointer Register */
#define EMAC_R_RXHDP(n)          0x620 /* Receive Channel n[0-7] DMA Head
										  Descriptor Pointer Register */
#define EMAC_R_TXCP(n)           0x640 /* Transmit Channel n[0-7] Completion
										  Pointer Register */
#define EMAC_R_RXCP(n)           0x660 /* Receive Channel n[0-7] Completion
										  Pointer Register */

#define EMAC_MACEOIVECTOR 0x2

/**
 * EMAC Network Header
 */
struct emac_hdr {
	uint8_t preamble[7]; /* 0x55 */
#define EMAC_HDR_PRE 0x55
	uint8_t sfd;         /* 0x5d */
#define EMAC_HDR_SFD 0x5d
};

/**
 * EMAC Buffer Descriptor
 */
struct emac_desc {
	uint32_t next;
	uint32_t data;
	uint16_t data_off;
	uint16_t data_len;
	uint16_t flags;
	uint16_t len;
};

/**
 * EMAC Descriptor Flags
 */
#define EMAC_DESC_F_SOP        0x8000U /* Start of Packet (SOP) Flag */
#define EMAC_DESC_F_EOP        0x4000U /* End of Packet (EOP) Flag */
#define EMAC_DESC_F_OWNER      0x2000U /* Ownership (OWNER) Flag */
#define EMAC_DESC_F_EOQ        0x1000U /* End of Queue (EOQ) Flag */
#define EMAC_DESC_F_TDOWNCMPLT 0x0800U /* Teardown Complete (TDOWNCMPLT) Flag */
#define EMAC_DESC_F_PASSCRC    0x0400U /* Pass CRC (PASSCRC) Flag */
#define EMAC_DESC_F_JABBER     0x0200U /* Jabber Flag */
#define EMAC_DESC_F_OVERSIZE   0x0100U /* Oversize Flag */
#define EMAC_DESC_F_FRAGMENT   0x0080U /* Fragment Flag */
#define EMAC_DESC_F_UNDERSIZED 0x0040U /* Undersized Flag */
#define EMAC_DESC_F_CONTROL    0x0020U /* Control Flag */
#define EMAC_DESC_F_OVERRUN    0x0010U /* Overrun Flag */
#define EMAC_DESC_F_CODEERROR  0x0008U /* Code Error (CODEERROR) Flag */
#define EMAC_DESC_F_ALIGNERROR 0x0004U /* Alignment Error (ALIGNERROR) Flag */
#define EMAC_DESC_F_CRCERROR   0x0002U /* CRC Error (CRCERROR) Flag */
#define EMAC_DESC_F_NOMATCH    0x0001U /* No Match (NOMATCH) Flag */

#endif /* DRIVERS_ETHERNET_TI816X_H_ */
