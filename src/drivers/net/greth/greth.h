/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 09.09.23
 */
#ifndef SRC_DRIVERS_NET_GRETH_GRETH_H_
#define SRC_DRIVERS_NET_GRETH_GRETH_H_

#include <stdint.h>

#include <net/netdevice.h>
#include <net/skbuff.h>

/**
 * Control register
 */
#define GRETH_CTRL_TE (1UL << 0)  /* Transmit enable */
#define GRETH_CTRL_RE (1UL << 1)  /* Receive enable */
#define GRETH_CTRL_TI (1UL << 2)  /* Transmit interrupt enable */
#define GRETH_CTRL_RI (1UL << 3)  /* Receive interrupt enable */
#define GRETH_CTRL_FD (1UL << 4)  /* Full duplex mode */
#define GRETH_CTRL_PM (1UL << 5)  /* Promiscuous mode */
#define GRETH_CTRL_RS (1UL << 6)  /* Reset */
#define GRETH_CTRL_SP (1UL << 7)  /* Speed: 0 = 10Mbit, 1 = 100Mbit */
#define GRETH_CTRL_GB (1UL << 7)  /* 0 = SP bit, 1 = 1000Mbit (GRETH_GBIT) */
#define GRETH_CTRL_PI (1UL << 10) /* PHY status change interrupt enable */
#define GRETH_CTRL_GA (1UL << 27) /* Gigabit MAC available */
#define GRETH_CTRL_ED (1UL << 31) /* EDCL available */

#define GRETH_CTRL_BS       /* EDCL buffer size (0=1кВ, 1=2кВ, ... 6=64кВ) */
#define GRETH_CTRL_BS_MASK  (7UL << GRETH_CTRL_BS_SHIFT)
#define GRETH_CTRL_BS_SHIFT 28

/**
 * Status/Interrupt-source register
 */
#define GRETH_STAT_RE (1UL << 0) /* Receiver error */
#define GRETH_STAT_TE (1UL << 1) /* Transmit error */
#define GRETH_STAT_RI (1UL << 2) /* Receiver interrupt */
#define GRETH_STAT_TI (1UL << 3) /* Transmitter interrupt */
#define GRETH_STAT_RA (1UL << 4) /* Receiver AHB error */
#define GRETH_STAT_TA (1UL << 5) /* Transmitter AHB error */
#define GRETH_STAT_TS (1UL << 6) /* Too small */
#define GRETH_STAT_IA (1UL << 7) /* Invalid address */

/**
 * MDIO Control/Status register
 */
#define GRETH_MDIO_WR (1UL << 0) /* Write */
#define GRETH_MDIO_RD (1UL << 1) /* Read */
#define GRETH_MDIO_LF (1UL << 2) /* Linkfail */
#define GRETH_MDIO_BU (1UL << 3) /* Busy */
#define GRETH_MDIO_NV (1UL << 4) /* Not valid */

#define GRETH_MDIO_RA       /* Register address */
#define GRETH_MDIO_RA_MASK  0x1fU
#define GRETH_MDIO_RA_SHIFT 6

#define GRETH_MDIO_PA       /* PHY address */
#define GRETH_MDIO_PA_MASK  0x1fU
#define GRETH_MDIO_PA_SHIFT 11

#define GRETH_MDIO_DT       /* Data */
#define GRETH_MDIO_DT_MASK  0xffffU
#define GRETH_MDIO_DT_SHIFT 16

#define GRETH_REGS(dev) ((struct greth_regs *)((dev)->base_addr))

struct greth_regs {
	uint32_t control;
	uint32_t status;
	uint32_t mac_msb;
	uint32_t mac_lsb;
	uint32_t mdio;
	uint32_t tx_desc_p;
	uint32_t rx_desc_p;
	uint32_t edcl_ip;
};

/**
 * Buffer descriptor (word 0)
 */
#define GRETH_BD_EN (1UL << 11) /* Enable */
#define GRETH_BD_WR (1UL << 12) /* Wrap */
#define GRETH_BD_IE (1UL << 13) /* Interrupt enable */

#define GRETH_TXBD_UE (1UL << 14) /* Underrun error */
#define GRETH_TXBD_AL (1UL << 15) /* Attempt limit error */

#define GRETH_RXBD_AE (1UL << 14) /* Alignment error */
#define GRETH_RXBD_FT (1UL << 15) /* Frame too long */
#define GRETH_RXBD_CE (1UL << 16) /* CRC error */
#define GRETH_RXBD_OE (1UL << 17) /* Overrun error */
#define GRETH_RXBD_LE (1UL << 18) /* Length error */

#define GRETH_BD_LE       /* Length */
#define GRETH_BD_LE_MASK  0x7ffU
#define GRETH_BD_LE_SHIFT 0

struct greth_bd {
	uint32_t stat;
	uint32_t addr;
};

struct greth_priv {
	struct sk_buff **rxbuf_base;
	struct greth_bd *txbd_base;
	struct greth_bd *rxbd_base;
	unsigned int rxbd_curr;
	uint8_t phy_id;
};

extern void __greth_dev_init(struct net_device *dev);

#endif /* SRC_DRIVERS_NET_GRETH_GRETH_H_ */
