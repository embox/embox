/**
 * @file
 * @brief Ethernet Media Access Controller for TMS320DM816x DaVinci
 *
 * @date 20.09.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_ETHERNET_TI816X_H_
#define DRIVERS_ETHERNET_TI816X_H_

#include <endian.h>
#include <net/mii.h>


#define EMAC_CHANNEL_COUNT 8

/**
 * EMAC Control Module Registers
 */
#define EMAC_R_CMIDVER           0x00 /* Identification and Version Register */
#define EMAC_R_CMSOFTRESET       0x04 /* Software Reset Register */
#define EMAC_R_CMEMCONTROL       0x08 /* Emulation Control Register */
#define EMAC_R_CMINTCTRL         0x0C /* Interrupt Control Register */
# define EMAC_CMINTCTRL_C0_TX (0x1 << 17) /* CMINTCTRL */
# define EMAC_CMINTCTRL_C0_RX (0x1 << 16)
# define EMAC_CMINTCTRL_INTPRESCALE(x) ((x & 0x7ff) << 0)

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
# define EMAC_CMRXINTMAX_RXIMAX(x) ((x & 0x3f) << 0) /* CMRXINTMAX */

#define EMAC_R_CMTXINTMAX        0x74 /* Transmit Interrupts Per Millisecond
										 Register */
# define EMAC_CMTXINTMAX_TXIMAX(x) ((x & 0x3f) << 0) /* CMTXINTMAX */

/**
 * EMAC Module Registers
 */
#define EMAC_R_CPGMACIDVER       0x000 /* Identification and Version Register */
#define EMAC_R_TXCONTROL         0x004 /* Transmit Control Register */
# define EMAC_TXCONTROL_TXEN (0x1 << 0)
#define EMAC_R_TXTEARDOWN        0x008 /* Transmit Teardown Register */
#define EMAC_R_RXCONTROL         0x014 /* Receive Control Register */
# define EMAC_RXCONTROL_RXEN (0x1 << 0)
#define EMAC_R_RXTEARDOWN        0x018 /* Receive Teardown Register */
#define EMAC_R_TXINTSTATRAW      0x080 /* Transmit Interrupt Status (Unmasked)
										  Register */
#define EMAC_R_TXINTSTATMASKED   0x084 /* Transmit Interrupt Status (Masked)
										  Register */
#define EMAC_R_TXINTMASKSET      0x088 /* Transmit Interrupt Mask Set
										  Register */
#define EMAC_R_TXINTMASKCLEAR    0x08C /* Transmit Interrupt Clear Register */

#define EMAC_R_MACINVECTOR       0x090 /* MAC Input Vector Register */
# define EMAC_MACINV_STATPEND  (0x1 << 27)
# define EMAC_MACINV_HOSTPEND  (0x1 << 26)
# define EMAC_MACINV_TXPEND    (DEFAULT_MASK << 16)
# define EMAC_MACINV_RXPEND    (DEFAULT_MASK << 0)

#define EMAC_R_MACEOIVECTOR      0x094 /* MAC End of Interrupt Vector
										  Register */
/* MACEOIVECTOR bits */
# define EMAC_MACEOIVEC_RXTHRESHEOI 0x0
# define EMAC_MACEOIVEC_RXEOI       0x1
# define EMAC_MACEOIVEC_TXEOI       0x2
# define EMAC_MACEOIVEC_MISCEOI     0x3

/* interrupts bits */
# define EMAC_MACINT_HOSTMASK  (0x1 << 1)
# define EMAC_MACINT_STATMASK  (0x1 << 0)

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
# define EMAC_RXMBP_RXNOCHAIN  (0x1 << 28) /* single buffer */
# define EMAC_RXMBP_RXCMFEN    (0x1 << 24) /* short msg */
# define EMAC_RXMBP_RXCSFEN    (0x1 << 23) /* short msg */
# define EMAC_RXMBP_RXCEFEN    (0x1 << 22) /* short msg */
# define EMAC_RXMBP_RXCAFEN    (0x1 << 21) /* promiscuous */
# define EMAC_RXMBP_RXBROADEN  (0x1 << 13) /* broadcast */
# define EMAC_RXMBP_RXMULTEN   (0x1 << 5) /* multicast */

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
# define EMAC_R_MACSTATUS         0x164 /* MAC Status Register */
# define EMAC_MACSTAT_IDLE(x)            ((x >> 31) & 0x1) /* MACSTATUS */
# define EMAC_MACSTAT_TXERRCODE(x)       ((x >> 20) & 0xf)
# define EMAC_MACSTAT_TXERRCH(x)         ((x >> 16) & 0x7)
# define EMAC_MACSTAT_RXERRCODE(x)       ((x >> 12) & 0xf)
# define EMAC_MACSTAT_RXERRCH(x)         ((x >> 8) & 0x7)
# define EMAC_MACSTAT_RGMIIGIG(x)        ((x >> 4) & 0x1)
# define EMAC_MACSTAT_RGMIIFULLDUPLEX(x) ((x >> 3) & 0x1)
# define EMAC_MACSTAT_RXQOSACT(x)        ((x >> 2) & 0x1)
# define EMAC_MACSTAT_RXFLOWACT(x)       ((x >> 1) & 0x1)
# define EMAC_MACSTAT_TXFLOWACT(x)       ((x >> 0) & 0x1)

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

/**
 * MDIO Registers
 */
#define MDIO_R_VERSION          0x00 /* MDIO Version Register */
#define MDIO_R_CONTROL          0x04 /* MDIO Control Register */
#define MDIO_R_ALIVE            0x08 /* PHY Alive Status register */
#define MDIO_R_LINK             0x0C /* PHY Link Status Register */
#define MDIO_R_LINKINTRAW       0x10 /* MDIO Link Status Change Interrupt
										(Unmasked) Register */
#define MDIO_R_LINKINTMASKED    0x14 /* MDIO Link Status Change Interrupt
										(Masked) Register */
#define MDIO_R_USERINTRAW       0x20 /* MDIO User Command Complete Interrupt
										(Unmasked) Register */
#define MDIO_R_USERINTMASKED    0x24 /* MDIO User Command Complete Interrupt
										(Masked) Register */
#define MDIO_R_USERINTMASKSET   0x28 /* MDIO User Command Complete Interrupt
										Mask Set Register */
#define MDIO_R_USERINTMASKCLEAR 0x2C /* MDIO User Command Complete Interrupt
										Mask Clear Register */
#define MDIO_R_USERACCESS0      0x80 /* MDIO User Access Register 0 */
#define MDIO_R_USERPHYSEL0      0x84 /* MDIO User PHY Select Register 0 */
#define MDIO_R_USERACCESS1      0x88 /* MDIO User Access Register 1 */
#define MDIO_R_USERPHYSEL1      0x8C /* MDIO User PHY Select Register 1 */

/* MDIO bits */
#define MDIO_CONTROL_IDLE            (0x80000000)
#define MDIO_CONTROL_ENABLE          (0x40000000)
#define MDIO_CONTROL_FAULT_ENABLE    (0x40000)
#define MDIO_CONTROL_FAULT           (0x80000)
#define MDIO_USERACCESS0_GO          (0x80000000)
#define MDIO_USERACCESS0_WRITE_READ  (0x0)
#define MDIO_USERACCESS0_WRITE_WRITE (0x40000000)
#define MDIO_USERACCESS0_ACK         (0x20000000)


/**
 * Control Module Base Address
 */
#define CM_BASE 0x48140000

/**
 * Control Module Registers
 */
#define CM_R_MACID0_LO 0x630 /* Ethernet MAC ID0 Low Register */
#define CM_R_MACID0_HI 0x634 /* Ethernet MAC ID0 High Register */

//TODO remove this when mdio is created
extern void emac_mdio_config(void);

#define EMAC_VERSION (OPTION_GET(NUMBER, version))

#include <framework/mod/options.h>
#if (OPTION_GET(NUMBER, speed) == 100)

#define MACCTRL_INIT (FULLDUPLEX | TXPACE)
#define RXMBP_INIT \
	(EMAC_RXMBP_RXNOCHAIN | EMAC_RXMBP_RXCSFEN | EMAC_RXMBP_RXCAFEN | EMAC_RXMBP_RXBROADEN | EMAC_RXMBP_RXMULTEN)
#define PHY_ADV (ADVERTISE_100FULL | ADVERTISE_100HALF)

#elif (OPTION_GET(NUMBER, speed) == 1000)

#define RXMBP_INIT \
	(EMAC_RXMBP_RXCMFEN | EMAC_RXMBP_RXCSFEN | EMAC_RXMBP_RXCEFEN | EMAC_RXMBP_RXCAFEN | EMAC_RXMBP_RXBROADEN | EMAC_RXMBP_RXMULTEN)
#define MACCTRL_INIT (FULLDUPLEX | GIG)
#define PHY_ADV (ADVERTISE_1000XFULL | ADVERTISE_1000XHALF)

#else
#error "setup ethernet speed"
#endif

void emac_mdio_enable(void);
void emac_detect_phy(void);
void emac_autonegotiate(void);
void emac_mdio_writereg(unsigned char reg_num,unsigned short data);
int emac_mdio_readreg(unsigned char reg_num);
void emac_mdelay(int value);
 void emac_set_macctrl(unsigned long v);

#if EMAC_VERSION == 1
#define LVL_INTR_CLEAR 0x48002594
#endif /* EMAC_VERSION == 1 */
#endif /* DRIVERS_ETHERNET_TI816X_H_ */
