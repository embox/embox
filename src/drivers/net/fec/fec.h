/**
 * @file imx6_net.h
 * @brief Flags, offsets and so on
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-05-14
 */

#ifndef DRIVERS_IMX6_NET_H_
#define DRIVERS_IMX6_NET_H_

#include <stdint.h>

#define NIC_BASE OPTION_GET(NUMBER, base_addr)
#define ENET_IRQ OPTION_GET(NUMBER, irq_nr)

#define ENET_EIR       (NIC_BASE + 0x0004)
#define ENET_EIMR      (NIC_BASE + 0x0008)
#define ENET_RDAR      (NIC_BASE + 0x0010)
#define ENET_TDAR      (NIC_BASE + 0x0014)
#define ENET_ECR       (NIC_BASE + 0x0024)
#define ENET_MMFR      (NIC_BASE + 0x0040)
#define ENET_MSCR      (NIC_BASE + 0x0044)
#define ENET_MIBC      (NIC_BASE + 0x0064)
#define ENET_RCR       (NIC_BASE + 0x0084)
#define ENET_TCR       (NIC_BASE + 0x00C4)
#define ENET_MAC_LOW   (NIC_BASE + 0x00E4)
#define ENET_MAC_HI    (NIC_BASE + 0x00E8)
#define ENET_OPD       (NIC_BASE + 0x00EC)
#define ENET_IAUR      (NIC_BASE + 0x0118)
#define ENET_IALR      (NIC_BASE + 0x011C)
#define ENET_GAUR      (NIC_BASE + 0x0120)
#define ENET_GALR      (NIC_BASE + 0x0124)
#define ENET_TFWR      (NIC_BASE + 0x0144)
#define ENET_WTF1      (NIC_BASE + 0x014C) /* Not mentioned             */
#define ENET_WTF2      (NIC_BASE + 0x0150) /*      in the specification */
#define ENET_RDSR      (NIC_BASE + 0x0180)
#define ENET_TDSR      (NIC_BASE + 0x0184)
#define ENET_MRBR      (NIC_BASE + 0x0188)
#define ENET_RAEM      (NIC_BASE + 0x0198)
#define ENET_RAFL      (NIC_BASE + 0x019C)
#define ENET_TAEM      (NIC_BASE + 0x01A4)
#define ENET_TAFL      (NIC_BASE + 0x01A8)
#define ENET_TIPG      (NIC_BASE + 0x01AC)
#define ENET_FTRL      (NIC_BASE + 0x01B0)


/* Various flags */
/* ENET_EIR */
#define ENET_EIR_MASK  0xFFFFFFFF
#define ENET_EIR_EBERR (1 << 22)   /* Ethernet bus error */
#define ENET_EIR_RXB   (1 << 24)   /* Receive buffer     */
#define ENET_EIR_RXF   (1 << 25)   /* Receive frame      */
#define ENET_EIR_TXB   (1 << 26)   /* Transmit buffer    */
#define ENET_EIR_TXF   (1 << 27)   /* Transmit frame     */

/* ENET_EIMR */
#define ENET_EIMR_RXF  (1 << 25)
#define ENET_EIMR_TXF  (1 << 27)

/* ENET_ECR */
#define ENET_ECR_DBSWP (1 << 8)
#define ENET_ETHEREN   (1 << 1) /* Ethernet enable */
#define ENET_RESET     (1 << 0)

/* ENET_RCR */
#define ENET_FRAME_LEN_OFFSET 16
#define ENET_RCR_FCE         (1 << 5)
#define ENET_RCR_MII_MODE    (1 << 2)

/* ENET_TCR */
#define ENET_TCR_FDEN (1 << 2)

/* ENET_TFWR */
#define ENET_TFWR_STRFWD (1 << 8)

#define FRAME_LEN     2048
#define TX_BUF_FRAMES 256
#define TX_BUF_LEN    (FRAME_LEN * TX_BUF_FRAMES)
#define RX_BUF_FRAMES 256
#define RX_BUF_LEN    (FRAME_LEN * RX_BUF_FRAMES)

/* Descriptor flags */
/* TX */
/* Flags 1 */
#define FLAG_R      (1 << 15)
#define FLAG_TO1    (1 << 14)
#define FLAG_W      (1 << 13)
#define FLAG_TO2    (1 << 12)
#define FLAG_L      (1 << 11)
#define FLAG_TC     (1 << 10)
/* Flags 2 */
#define FLAG_TSE    (1 << 8)
#define FLAG_OE     (1 << 9)
#define FLAG_LCE    (1 << 10)
#define FLAG_FE     (1 << 11)
#define FLAG_EE     (1 << 12)
#define FLAG_UE     (1 << 13)
#define FLAG_TXE    (1 << 15)
#define FLAG_IINS   (1 << 27)
#define FLAG_PINS   (1 << 28)
#define FLAG_TS     (1 << 29)
#define FLAG_INT_TX (1 << 30)
/* Flags 3 */
#define FLAG_BDU    (1 << 15)

/* RX */
/* Flags 1 */
#define FLAG_TR     (1 << 0)
#define FLAG_OV     (1 << 1)
#define FLAG_CR     (1 << 2)
#define FLAG_NO     (1 << 4)
#define FLAG_LG     (1 << 5)
#define FLAG_MC     (1 << 6)
#define FLAG_BC     (1 << 7)
#define FLAG_M      (1 << 8)
#define FLAG_RO2    (1 << 12)
#define FLAG_RO1    (1 << 14)
#define FLAG_E      (1 << 15)
/* Flags 2 */
#define FLAG_FRAG   (1 << 0)
#define FLAG_IPV6   (1 << 1)
#define FLAG_VLAN   (1 << 2)
#define FLAG_PCR    (1 << 4)
#define FLAG_ICE    (1 << 5)
#define FLAG_INT_RX (1 << 23)
#define FLAG_UC     (1 << 24)
#define FLAG_CE     (1 << 25)
#define FLAG_PE     (1 << 26)
#define FLAG_ME     (1 << 31)

struct fec_buf_desc {
	uint16_t len;
	uint16_t flags;
	uint32_t data_pointer;
#if 0
	uint32_t flags2;
	uint16_t payload_checksum; /* Used only for RX */
	uint8_t  protocol;         /* Used only for RX */
	uint8_t  header_len;       /* Used only for RX */
	uint16_t pad1;             /* Unused */
	uint16_t flags3;           /* Used only for RX */
	uint32_t timestamp;
	uint16_t pad2[4];          /* Unused */
#endif
};


/* the defins of MII operation */
#define FEC_MII_ST      0x40000000
#define FEC_MII_OP_OFF  28
#define FEC_MII_OP_MASK 0x03
#define FEC_MII_OP_RD   0x02
#define FEC_MII_OP_WR   0x01
#define FEC_MII_PA_OFF  23
#define FEC_MII_PA_MASK 0xFF
#define FEC_MII_RA_OFF  18
#define FEC_MII_RA_MASK 0xFF
#define FEC_MII_TA      0x00020000
#define FEC_MII_DATA_OFF 0
#define FEC_MII_DATA_MASK 0x0000FFFF

#define FEC_MII_FRAME   \
	(FEC_MII_ST | FEC_MII_TA)
#define FEC_MII_OP(x)   \
	(((x) & FEC_MII_OP_MASK) << FEC_MII_OP_OFF)
#define FEC_MII_PA(pa)  \
	(((pa) & FEC_MII_PA_MASK) << FEC_MII_PA_OFF)
#define FEC_MII_RA(ra)  \
	(((ra) & FEC_MII_RA_MASK) << FEC_MII_RA_OFF)
#define FEC_MII_SET_DATA(v) \
	(((v) & FEC_MII_DATA_MASK) << FEC_MII_DATA_OFF)
#define FEC_MII_GET_DATA(v) \
	(((v) >> FEC_MII_DATA_OFF) & FEC_MII_DATA_MASK)
#define FEC_MII_READ(pa, ra) \
	((FEC_MII_FRAME | FEC_MII_OP(FEC_MII_OP_RD)) | FEC_MII_PA(pa) | FEC_MII_RA(ra))
#define FEC_MII_WRITE(pa, ra, v) \
	(FEC_MII_FRAME | FEC_MII_OP(FEC_MII_OP_WR)|	FEC_MII_PA(pa) | FEC_MII_RA(ra) | FEC_MII_SET_DATA(v))

#endif /* DRIVERS_IMX6_NET_H_ */
