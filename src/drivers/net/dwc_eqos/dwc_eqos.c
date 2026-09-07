/**
 * @file
 * @brief Synopsys DesignWare Ethernet QoS (dwmac-4.x) core driver.
 *
 * One combined mac/dma interrupt per port, a single RX/TX channel with
 * tail-pointer ring arming, and MDIO through the dwmac4 register block.
 * The generic Embox PHY layer (drivers/net/phy) drives autonegotiation
 * through the mdio_read/mdio_write/set_speed hooks.  Everything
 * SoC-specific (clock trees, pin iomux, PHY reset GPIOs) is behind the
 * soc_init/soc_swr_quirk hooks of struct dwc_eqos_plat.
 *
 * Descriptor rings and frame buffers are static arrays shared with the
 * DMA through explicit cache maintenance: every descriptor is flushed
 * after the software writes it and invalidated before the software
 * reads the hardware writeback.
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <framework/mod/options.h>
#include <hal/cache.h>
#include <hal/ipl.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/sys_timer.h>
#include <kernel/time/time.h>
#include <util/log.h>

#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/inetdevice.h>
#include <net/mii.h>
#include <net/netdevice.h>
#include <net/phy.h>
#include <net/skbuff.h>

#include "dwc_eqos.h"

#define RX_DESC_QUANTITY  OPTION_GET(NUMBER, rx_desc_quantity)
#define TX_DESC_QUANTITY  OPTION_GET(NUMBER, tx_desc_quantity)
#define RX_BUF_SIZE       OPTION_GET(NUMBER, rx_buf_size)
#define PCLK_HZ           OPTION_GET(NUMBER, pclk_hz)
#define TXPBL             OPTION_GET(NUMBER, txpbl)
#define RXPBL             OPTION_GET(NUMBER, rxpbl)

#define PORT_NUM          2

/* Per-frame DMA buffers: every buffer gets its own 2K slot so that
 * cache maintenance on one buffer never touches a neighbour. */
#define DMA_BUF_ALIGN     0x800
#define DMA_BUF_STRIDE    0x800

struct dwc_eqos_priv {
	const struct dwc_eqos_plat *plat;
	struct net_device    *dev;
	unsigned long         base_addr;
	uint8_t               macaddr[ETH_ALEN];
	int                   phy_id;

	struct dwc_eqos_desc  *rx_ring;
	struct dwc_eqos_desc  *tx_ring;
	int                   tx_head;
	int                   tx_tail;
	/* Software ownership handoff per rx slot: a slot is delivered
	 * and refilled wherever its writeback shows up in the ring,
	 * because the rx dma laps past the software instead of
	 * stopping at a single head position (see eqos_rx_complete). */
	uint8_t               rx_in_flight[RX_DESC_QUANTITY];

	struct sys_timer      rx_poll;
};

static struct dwc_eqos_priv eqos_priv[PORT_NUM];

static struct dwc_eqos_desc rx_rings[PORT_NUM][RX_DESC_QUANTITY]
		__attribute__((aligned(DMA_BUF_ALIGN)));
static struct dwc_eqos_desc tx_rings[PORT_NUM][TX_DESC_QUANTITY]
		__attribute__((aligned(DMA_BUF_ALIGN)));

static uint8_t rx_buffers[PORT_NUM][RX_DESC_QUANTITY][DMA_BUF_STRIDE]
		__attribute__((aligned(DMA_BUF_ALIGN)));
static uint8_t tx_buffers[PORT_NUM][TX_DESC_QUANTITY][DMA_BUF_STRIDE]
		__attribute__((aligned(DMA_BUF_ALIGN)));

static inline uint32_t eqos_read(const struct dwc_eqos_priv *priv,
		unsigned int reg) {
	return REG32_LOAD(priv->base_addr + reg);
}

static inline void eqos_write(const struct dwc_eqos_priv *priv,
		unsigned int reg, uint32_t val) {
	REG32_STORE(priv->base_addr + reg, val);
}

static void eqos_set_macaddr_regs(const struct dwc_eqos_priv *priv,
		const uint8_t *addr) {
	eqos_write(priv, DWC_EQOS_MAC_ADDRESS0_LOW,
			addr[0] | (addr[1] << 8) | (addr[2] << 16) | (addr[3] << 24));
	eqos_write(priv, DWC_EQOS_MAC_ADDRESS0_HIGH,
			addr[4] | (addr[5] << 8));
}

static int eqos_mdio_read(struct net_device *dev, uint8_t reg_addr) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;
	uint32_t mii_addr;
	int cnt;

	mii_addr = ((uint32_t) priv->phy_id << DWC_EQOS_MDIO_PA_SHIFT)
			| ((uint32_t) reg_addr << DWC_EQOS_MDIO_RDA_SHIFT)
			| (DWC_EQOS_MDIO_CR_150_250M << DWC_EQOS_MDIO_CR_SHIFT)
			| DWC_EQOS_MDIO_GOC_READ
			| DWC_EQOS_MDIO_GB;

	eqos_write(priv, DWC_EQOS_MAC_MDIO_ADDRESS, mii_addr);
	for (cnt = 0; cnt < 100000; cnt++) {
		if (!(eqos_read(priv, DWC_EQOS_MAC_MDIO_ADDRESS) & DWC_EQOS_MDIO_GB)) {
			return eqos_read(priv, DWC_EQOS_MAC_MDIO_DATA) & 0xffff;
		}
	}

	log_error("mdio read %#x timeout", reg_addr);
	return -ETIMEDOUT;
}

static int eqos_mdio_write(struct net_device *dev, uint8_t reg_addr,
		uint16_t data) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;
	uint32_t mii_addr;
	int cnt;

	eqos_write(priv, DWC_EQOS_MAC_MDIO_DATA, data);
	mii_addr = ((uint32_t) priv->phy_id << DWC_EQOS_MDIO_PA_SHIFT)
			| ((uint32_t) reg_addr << DWC_EQOS_MDIO_RDA_SHIFT)
			| (DWC_EQOS_MDIO_CR_150_250M << DWC_EQOS_MDIO_CR_SHIFT)
			| DWC_EQOS_MDIO_GOC_WRITE
			| DWC_EQOS_MDIO_GB;

	eqos_write(priv, DWC_EQOS_MAC_MDIO_ADDRESS, mii_addr);
	for (cnt = 0; cnt < 100000; cnt++) {
		if (!(eqos_read(priv, DWC_EQOS_MAC_MDIO_ADDRESS) & DWC_EQOS_MDIO_GB)) {
			return 0;
		}
	}

	log_error("mdio write %#x timeout", reg_addr);
	return -ETIMEDOUT;
}

/* Raw MDIO helpers for the pre-mii PHY poke, phy id passed explicitly */
static int eqos_mdio_raw_read(struct dwc_eqos_priv *priv, int phy,
		uint8_t reg_addr, uint32_t *val) {
	uint32_t mii_addr;
	int cnt;

	mii_addr = ((uint32_t) phy << DWC_EQOS_MDIO_PA_SHIFT)
			| ((uint32_t) reg_addr << DWC_EQOS_MDIO_RDA_SHIFT)
			| (DWC_EQOS_MDIO_CR_150_250M << DWC_EQOS_MDIO_CR_SHIFT)
			| DWC_EQOS_MDIO_GOC_READ
			| DWC_EQOS_MDIO_GB;

	eqos_write(priv, DWC_EQOS_MAC_MDIO_ADDRESS, mii_addr);
	for (cnt = 0; cnt < 100000; cnt++) {
		if (!(eqos_read(priv, DWC_EQOS_MAC_MDIO_ADDRESS) & DWC_EQOS_MDIO_GB)) {
			*val = eqos_read(priv, DWC_EQOS_MAC_MDIO_DATA) & 0xffff;
			return 0;
		}
	}

	return -ETIMEDOUT;
}

static int eqos_mdio_raw_write(struct dwc_eqos_priv *priv, int phy,
		uint8_t reg_addr, uint16_t data) {
	uint32_t mii_addr;
	int cnt;

	eqos_write(priv, DWC_EQOS_MAC_MDIO_DATA, data);
	mii_addr = ((uint32_t) phy << DWC_EQOS_MDIO_PA_SHIFT)
			| ((uint32_t) reg_addr << DWC_EQOS_MDIO_RDA_SHIFT)
			| (DWC_EQOS_MDIO_CR_150_250M << DWC_EQOS_MDIO_CR_SHIFT)
			| DWC_EQOS_MDIO_GOC_WRITE
			| DWC_EQOS_MDIO_GB;

	eqos_write(priv, DWC_EQOS_MAC_MDIO_ADDRESS, mii_addr);
	for (cnt = 0; cnt < 100000; cnt++) {
		if (!(eqos_read(priv, DWC_EQOS_MAC_MDIO_ADDRESS) & DWC_EQOS_MDIO_GB)) {
			return 0;
		}
	}

	return -ETIMEDOUT;
}

static void eqos_set_phyid(struct net_device *dev, uint8_t phyid) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;

	priv->phy_id = phyid;
}

static int eqos_set_speed(struct net_device *dev, int speed) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;
	uint32_t reg;

	reg = eqos_read(priv, DWC_EQOS_MAC_CONFIGURATION);

	/* Frame burst enable, jabber disable, jumbo enable, disable CRC
	 * status removal: the received frames keep their FCS, which the
	 * rx path strips manually (same configuration as the proven
	 * netbsd eqos setup on this board). */
	reg |= DWC_EQOS_MAC_CONF_BE | DWC_EQOS_MAC_CONF_JD
			| DWC_EQOS_MAC_CONF_JE | DWC_EQOS_MAC_CONF_DCRS;

	speed = net_to_mbps(speed);
	switch (speed) {
	case 1000:
		reg &= ~(DWC_EQOS_MAC_CONF_PS | DWC_EQOS_MAC_CONF_FES);
		break;
	case 100:
		reg |= DWC_EQOS_MAC_CONF_PS | DWC_EQOS_MAC_CONF_FES;
		break;
	case 10:
		reg |= DWC_EQOS_MAC_CONF_PS;
		reg &= ~DWC_EQOS_MAC_CONF_FES;
		break;
	default:
		log_error("unsupported speed %d", speed);
		return -EINVAL;
	}

	reg |= DWC_EQOS_MAC_CONF_DM;
	eqos_write(priv, DWC_EQOS_MAC_CONFIGURATION, reg);

	log_info("link speed %d Mbps full duplex", speed);

	return 0;
}

/* Clear the RTL8211F strapped RGMII TX-delay after the hard reset
 * pulse re-latched it.  The board GRF delays are calibrated with the
 * PHY-side delay off; leaving the strap set double-delays the TX path
 * and long frames never reach the wire.  Raw MDIO, before the PHY
 * layer attaches. */
static void eqos_phy_tx_delay_clear(struct dwc_eqos_priv *priv) {
	uint32_t id1, id2, val;
	int phy, res;

	for (phy = 0; phy < 32; phy++) {
		res = eqos_mdio_raw_read(priv, phy, MII_PHYSID1, &id1);
		if (res) {
			continue;
		}
		res = eqos_mdio_raw_read(priv, phy, MII_PHYSID2, &id2);
		if (res) {
			continue;
		}
		if (id1 != RTL8211F_PHYSID1 || id2 != RTL8211F_PHYSID2) {
			continue;
		}

		log_info("RTL8211F phy %d: id %#x:%#x", phy, id1, id2);

		/* switch to page d08 */
		eqos_mdio_raw_write(priv, phy, RTL8211F_PAGESEL, 0x0d08);
		if (eqos_mdio_raw_read(priv, phy, 0x11, &val) == 0) {
			if (val & RTL8211F_TXDELAY) {
				eqos_mdio_raw_write(priv, phy, 0x11,
						val & ~RTL8211F_TXDELAY);
				log_info("RTL8211F phy %d: cleared strapped RGMII"
						" TX-delay (d08.0x11 %#x -> %#x)",
						phy, val, val & ~RTL8211F_TXDELAY);
			}
		}
		/* back to page 0 */
		eqos_mdio_raw_write(priv, phy, RTL8211F_PAGESEL, 0);
	}
}

static int eqos_hw_stop(struct dwc_eqos_priv *priv) {
	uint32_t reg;

	eqos_write(priv, DWC_EQOS_DMA_CH0_INTR_ENABLE, 0);

	reg = eqos_read(priv, DWC_EQOS_DMA_CH0_TX_CONTROL);
	reg &= ~DWC_EQOS_TX_CTRL_ST;
	eqos_write(priv, DWC_EQOS_DMA_CH0_TX_CONTROL, reg);

	reg = eqos_read(priv, DWC_EQOS_DMA_CH0_RX_CONTROL);
	reg &= ~DWC_EQOS_RX_CTRL_SR;
	eqos_write(priv, DWC_EQOS_DMA_CH0_RX_CONTROL, reg);

	reg = eqos_read(priv, DWC_EQOS_MAC_CONFIGURATION);
	reg &= ~(DWC_EQOS_MAC_CONF_TE | DWC_EQOS_MAC_CONF_RE);
	eqos_write(priv, DWC_EQOS_MAC_CONFIGURATION, reg);

	return 0;
}

static int eqos_hw_reset(struct dwc_eqos_priv *priv) {
	const struct dwc_eqos_plat *plat = priv->plat;
	uint32_t mode;
	int cnt, cleared = 0;

	log_debug("dma_mode before swr: %#x",
			eqos_read(priv, DWC_EQOS_DMA_MODE));

	if (plat->soc_swr_quirk != NULL) {
		plat->soc_swr_quirk(plat, 1);
	}

	eqos_write(priv, DWC_EQOS_DMA_MODE, DWC_EQOS_DMA_MODE_SWR);
	/* The reset normally clears within a few dma clock cycles: spin
	 * on the register first, then fall back to sleeping. */
	for (cnt = 0; (cnt < 1000000) && !cleared; cnt++) {
		cleared = !(eqos_read(priv, DWC_EQOS_DMA_MODE)
				& DWC_EQOS_DMA_MODE_SWR);
	}
	for (cnt = 0; (cnt < 500) && !cleared; cnt++) {
		usleep(10 * USEC_PER_MSEC);
		cleared = !(eqos_read(priv, DWC_EQOS_DMA_MODE)
				& DWC_EQOS_DMA_MODE_SWR);
	}

	if (plat->soc_swr_quirk != NULL) {
		plat->soc_swr_quirk(plat, 0);
	}

	if (!cleared) {
		mode = eqos_read(priv, DWC_EQOS_DMA_MODE);
		log_error("dma soft reset timeout, mode=%#x", mode);
		return -EIO;
	}

	return 0;
}

static void eqos_setup_rx_desc(struct dwc_eqos_priv *priv, int idx) {
	struct dwc_eqos_desc *desc = &priv->rx_ring[idx];
	uint8_t *buf = rx_buffers[priv->plat->idx][idx];

	/* The buffer was written by software (bss init) or by the previous
	 * DMA transfer: make sure no stale dirty line survives before the
	 * dma starts writing into it. */
	dcache_flush(buf, RX_BUF_SIZE);
	dcache_inval(buf, RX_BUF_SIZE);

	desc->des0 = (uint32_t) ((uintptr_t) buf);
	desc->des1 = 0;
	desc->des2 = 0;
	desc->des3 = DWC_EQOS_DESC_OWN | DWC_EQOS_DESC_RX_IOC | DWC_EQOS_DESC_RX_BUF1V;
	dcache_flush(desc, sizeof(*desc));
}

static int eqos_hw_start(struct dwc_eqos_priv *priv) {
	uint32_t reg;

	/* Channel: PBLx8, contiguous 16-byte descriptors (no skip) */
	reg = eqos_read(priv, DWC_EQOS_DMA_CH0_CONTROL);
	reg |= DWC_EQOS_CH0_CTRL_PBLX8;
	eqos_write(priv, DWC_EQOS_DMA_CH0_CONTROL, reg);

	eqos_write(priv, DWC_EQOS_DMA_CH0_TX_CONTROL,
			(TXPBL << DWC_EQOS_TX_CTRL_TXPBL_SHIFT) | DWC_EQOS_TX_CTRL_OSP);
	eqos_write(priv, DWC_EQOS_DMA_CH0_RX_CONTROL,
			(RXPBL << DWC_EQOS_RX_CTRL_RXPBL_SHIFT)
			| (RX_BUF_SIZE << DWC_EQOS_RX_CTRL_RBSZ_SHIFT));

	/* RX end-pointer arming: on this core the fetch window is
	 * [ring base, end address) -- the dma wraps to the ring base
	 * once it reaches the end and never fetches the end descriptor
	 * itself. Point it at the last slot once and leave it alone:
	 * moving the boundary with every refill is what wedges the
	 * ring when the dma catches up with the software. */
	eqos_write(priv, DWC_EQOS_DMA_CH0_RX_END_ADDR,
			(uint32_t) (uintptr_t) &priv->rx_ring[RX_DESC_QUANTITY - 1]);

	/* Enable channel interrupts, then start both DMA engines. */
	eqos_write(priv, DWC_EQOS_DMA_CH0_INTR_ENABLE, DWC_EQOS_CH0_INTR_DEFAULT);

	eqos_write(priv, DWC_EQOS_DMA_CH0_TX_CONTROL,
			eqos_read(priv, DWC_EQOS_DMA_CH0_TX_CONTROL)
			| DWC_EQOS_TX_CTRL_ST);
	eqos_write(priv, DWC_EQOS_DMA_CH0_RX_CONTROL,
			eqos_read(priv, DWC_EQOS_DMA_CH0_RX_CONTROL)
			| DWC_EQOS_RX_CTRL_SR);

	/* MAC transmit/receive enable */
	reg = eqos_read(priv, DWC_EQOS_MAC_CONFIGURATION);
	reg |= DWC_EQOS_MAC_CONF_TE | DWC_EQOS_MAC_CONF_RE;
	eqos_write(priv, DWC_EQOS_MAC_CONFIGURATION, reg);

	return 0;
}

static int eqos_hw_init(struct dwc_eqos_priv *priv) {
	uint32_t reg, hw_feat;
	int res, i;

	/* Stop the tx/rx engines and mask all channel interrupts before
	 * the soft reset (the firmware may have left them running). */
	eqos_hw_stop(priv);

	res = eqos_hw_reset(priv);
	if (res) {
		return res;
	}

	/* AXI bus: outstanding request limits and 4/8/16-beat bursts. */
	eqos_write(priv, DWC_EQOS_DMA_SYSBUS_MODE,
			(4u << DWC_EQOS_SYSBUS_WR_OSR_SHIFT)
			| (8u << DWC_EQOS_SYSBUS_RD_OSR_SHIFT)
			| DWC_EQOS_SYSBUS_EAME
			| DWC_EQOS_SYSBUS_BLEN16 | DWC_EQOS_SYSBUS_BLEN8
			| DWC_EQOS_SYSBUS_BLEN4);

	/* MTL queue sizes from the hardware feature registers: the fifo
	 * size is encoded as log2(n/128) and the TQS/RQS fields hold
	 * fifo/256 - 1. */
	hw_feat = eqos_read(priv, DWC_EQOS_MAC_HW_FEATURE1);
	reg = DWC_EQOS_MTL_TXQ0_OP_TXQEN_EN | DWC_EQOS_MTL_TXQ0_OP_TSF;
	reg |= ((((128u << ((hw_feat & DWC_EQOS_HW_TXFIFOSZ_MASK)
					>> DWC_EQOS_HW_TXFIFOSZ_SHIFT)) >> 8) - 1u)
			<< DWC_EQOS_MTL_TXQ0_OP_TQS_SHIFT)
			& DWC_EQOS_MTL_TXQ0_OP_TQS_MASK;
	eqos_write(priv, DWC_EQOS_MTL_TXQ0_OPERATION_MODE, reg);

	reg = DWC_EQOS_MTL_RXQ0_OP_RSF | DWC_EQOS_MTL_RXQ0_OP_FEP
			| DWC_EQOS_MTL_RXQ0_OP_FUP;
	reg |= ((((128u << (hw_feat & DWC_EQOS_HW_RXFIFOSZ_MASK)) >> 8) - 1u)
			<< DWC_EQOS_MTL_RXQ0_OP_RQS_SHIFT)
			& DWC_EQOS_MTL_RXQ0_OP_RQS_MASK;
	eqos_write(priv, DWC_EQOS_MTL_RXQ0_OPERATION_MODE, reg);

	/* Route all traffic to RX queue 0, MC/BC filter to queue 0. */
	eqos_write(priv, DWC_EQOS_MAC_RXQ_CTRL0, DWC_EQOS_RXQ_CTRL0_RXQ0EN_DCB);
	reg = eqos_read(priv, DWC_EQOS_MAC_RXQ_CTRL1);
	reg |= DWC_EQOS_RXQ_CTRL1_MCBCQEN;
	eqos_write(priv, DWC_EQOS_MAC_RXQ_CTRL1, reg);

	/* Flow control off, promiscuous reception, ~1us tick for the
	 * watchdog timers. */
	eqos_write(priv, DWC_EQOS_MAC_Q0_TX_FLOW_CTRL, 0);
	eqos_write(priv, DWC_EQOS_MAC_RX_FLOW_CTRL, 0);
	eqos_write(priv, DWC_EQOS_MAC_PACKET_FILTER, DWC_EQOS_PKT_FLT_PR);
	eqos_write(priv, DWC_EQOS_MAC_1US_TIC_COUNTER, PCLK_HZ / 1000000 - 1);

	eqos_set_macaddr_regs(priv, priv->macaddr);
	memcpy(priv->dev->dev_addr, priv->macaddr, ETH_ALEN);

	/* Descriptor rings: static arrays, made visible to the dma with
	 * explicit cache maintenance. */
	memset(priv->rx_ring, 0,
			RX_DESC_QUANTITY * sizeof(struct dwc_eqos_desc));
	memset(priv->tx_ring, 0,
			TX_DESC_QUANTITY * sizeof(struct dwc_eqos_desc));
	dcache_flush(priv->rx_ring,
			RX_DESC_QUANTITY * sizeof(struct dwc_eqos_desc));
	dcache_flush(priv->tx_ring,
			TX_DESC_QUANTITY * sizeof(struct dwc_eqos_desc));
	memset(priv->rx_in_flight, 1, RX_DESC_QUANTITY);
	priv->tx_head = 0;
	priv->tx_tail = 0;

	for (i = 0; i < RX_DESC_QUANTITY; i++) {
		eqos_setup_rx_desc(priv, i);
	}

	eqos_write(priv, DWC_EQOS_DMA_CH0_TX_BASE_ADDR_HI, 0);
	eqos_write(priv, DWC_EQOS_DMA_CH0_TX_BASE_ADDR_LO,
			(uint32_t) (uintptr_t) priv->tx_ring);
	eqos_write(priv, DWC_EQOS_DMA_CH0_RX_BASE_ADDR_HI, 0);
	eqos_write(priv, DWC_EQOS_DMA_CH0_RX_BASE_ADDR_LO,
			(uint32_t) (uintptr_t) priv->rx_ring);
	eqos_write(priv, DWC_EQOS_DMA_CH0_TX_RING_LEN, TX_DESC_QUANTITY - 1);
	eqos_write(priv, DWC_EQOS_DMA_CH0_RX_RING_LEN, RX_DESC_QUANTITY - 1);

	return 0;
}

static int eqos_rx_complete(struct net_device *dev) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;
	struct sk_buff *skb;
	struct dwc_eqos_desc *desc;
	uint8_t *buf;
	int len, i;

	/* The rx dma laps the ring: once its fetch pointer reaches the
	 * end address it wraps to the ring base, so writebacks land
	 * wherever the next frame went, not at a moving head. Sweep
	 * every slot with an outstanding handoff instead and deliver
	 * each frame from the slot its writeback actually landed in. */
	for (i = 0; i < RX_DESC_QUANTITY; i++) {
		if (!priv->rx_in_flight[i]) {
			continue;
		}
		desc = &priv->rx_ring[i];
		dcache_inval(desc, sizeof(*desc));
		if (desc->des3 & DWC_EQOS_DESC_OWN) {
			continue;
		}

		priv->rx_in_flight[i] = 0;
		buf = rx_buffers[priv->plat->idx][i];
		len = desc->des3 & DWC_EQOS_DESC_RX_LEN_MASK;

		if (len < 4) {
			log_error("runt frame, len %d", len);
		} else {
			dcache_inval(buf, len);

			/* The skb data pool holds ETH_FRAME_LEN bytes:
			 * strip the frame check sequence before
			 * copying. */
			len -= 4;
			skb = skb_alloc(len);
			if (skb == NULL) {
				log_error("no skb for rx frame");
			} else {
				memcpy(skb_data_cast_in(skb->data), buf, len);
				skb->len = len;
				skb->dev = dev;

				netif_rx(skb);
			}
		}

		eqos_setup_rx_desc(priv, i);
		priv->rx_in_flight[i] = 1;
	}

	return 0;
}

static void eqos_tx_complete(struct dwc_eqos_priv *priv) {
	struct dwc_eqos_desc *desc;
	int cnt;

	for (cnt = 0; cnt < TX_DESC_QUANTITY; cnt++) {
		if (priv->tx_tail == priv->tx_head) {
			break;
		}
		desc = &priv->tx_ring[priv->tx_tail];
		dcache_inval(desc, sizeof(*desc));
		if (desc->des3 & DWC_EQOS_DESC_OWN) {
			break;
		}
		priv->tx_tail = (priv->tx_tail + 1) % TX_DESC_QUANTITY;
	}

	if (priv->tx_tail != priv->tx_head) {
		/* resume the dma if it stopped on an unavailable buffer */
		eqos_write(priv, DWC_EQOS_DMA_CH0_TX_END_ADDR,
				(uint32_t) (uintptr_t) &priv->tx_ring[priv->tx_head]);
	}
}

/* Belt and braces for the rx path: even a single lost rx interrupt
 * would leave freshly received frames sitting in the ring until the
 * next one arrives, and a dma suspended on a not-yet-refilled slot
 * needs the end register rewritten to resume. Reclaim on a slow
 * timer as well; ipl_save serializes this against the irq handler. */
static void eqos_rx_poll(struct sys_timer *timer, void *param) {
	struct dwc_eqos_priv *priv = param;
	ipl_t ipl;

	ipl = ipl_save();
	{
		eqos_rx_complete(priv->dev);
		eqos_tx_complete(priv);
		eqos_write(priv, DWC_EQOS_DMA_CH0_RX_END_ADDR,
				(uint32_t) (uintptr_t)
				&priv->rx_ring[RX_DESC_QUANTITY - 1]);
	}
	ipl_restore(ipl);
}

static irq_return_t eqos_irq_handler(unsigned int irq_num, void *dev_id) {
	struct net_device *dev = dev_id;
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;
	uint32_t status;
	int pass;

	for (pass = 0; pass < 8; pass++) {
		status = eqos_read(priv, DWC_EQOS_DMA_CH0_STATUS);
		if (status == 0) {
			break;
		}
		/* write-one-to-clear everything latched: leaving e.g. the
		 * abnormal interrupt summary set would keep the line
		 * asserted forever */
		eqos_write(priv, DWC_EQOS_DMA_CH0_STATUS, status);

		if (!(status & (DWC_EQOS_CH0_STATUS_RI | DWC_EQOS_CH0_STATUS_TI
						| DWC_EQOS_CH0_STATUS_FB))) {
			log_debug("%s: dma status %#x", dev->name, status);
			break;
		}

		if (status & DWC_EQOS_CH0_STATUS_FB) {
			/* Fatal bus error: shut the channel down (no sleeping
			 * is allowed here, so no full re-init). */
			log_error("dma fatal bus error, stopping %s", dev->name);
			eqos_hw_stop(priv);
			return IRQ_HANDLED;
		}

		if (status & DWC_EQOS_CH0_STATUS_RI) {
			eqos_rx_complete(dev);
		}

		if (status & DWC_EQOS_CH0_STATUS_TI) {
			eqos_tx_complete(priv);
		}
	}

	return IRQ_HANDLED;
}

static int eqos_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;
	struct dwc_eqos_desc *desc;
	uint8_t *buf;
	ipl_t ipl;
	unsigned int tx_len;
	int cur, next;

	if (skb->len > RX_BUF_SIZE) {
		log_error("tx frame too long %d", skb->len);
		skb_free(skb);
		return -EINVAL;
	}

	ipl = ipl_save();
	{
		cur = priv->tx_head;
		next = (cur + 1) % TX_DESC_QUANTITY;
		if (next == priv->tx_tail) {
			ipl_restore(ipl);
			log_error("tx ring full, dropping frame");
			skb_free(skb);
			return -ENOMEM;
		}

		buf = tx_buffers[priv->plat->idx][cur];
		memcpy(buf, skb_data_cast_in(skb->data), skb->len);

		/* Ethernet requires a minimum frame of 60 bytes before the
		 * FCS: shorter frames (e.g. 42-byte ARP) are discarded as
		 * runts by every receiver, so zero-pad them here. */
		tx_len = skb->len;
		if (tx_len < ETH_ZLEN) {
			memset(buf + skb->len, 0, ETH_ZLEN - skb->len);
			tx_len = ETH_ZLEN;
		}
		dcache_flush(buf, tx_len);

		desc = &priv->tx_ring[cur];
		desc->des0 = (uint32_t) ((uintptr_t) buf);
		desc->des1 = 0;
		/* On this core the tx descriptor carries the frame length
		 * in des3 (like the rx writeback) and the completion
		 * interrupt enable in des2 bit31. */
		desc->des2 = DWC_EQOS_DESC_TX_IOC | (tx_len & 0x3fffu);
		desc->des3 = DWC_EQOS_DESC_OWN | DWC_EQOS_DESC_TX_FD | DWC_EQOS_DESC_TX_LD
				| tx_len;
		dcache_flush(desc, sizeof(*desc));

		priv->tx_head = next;

		/* Kick the dma: the fetch window is [ring base, end), so
		 * the end must point past the descriptor just armed. */
		eqos_write(priv, DWC_EQOS_DMA_CH0_TX_END_ADDR,
				(uint32_t) (uintptr_t)
				&priv->tx_ring[priv->tx_head]);
	}
	ipl_restore(ipl);

	skb_free(skb);

	return 0;
}

static int eqos_open(struct net_device *dev) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;
	int id1, id2, bmsr;
	int a;

	log_info("%s: opening", dev->name);
	for (a = 0; a < 8; a++) {
		priv->phy_id = a;
		id1 = eqos_mdio_read(dev, MII_PHYSID1);
		id2 = eqos_mdio_read(dev, MII_PHYSID2);
		bmsr = eqos_mdio_read(dev, MII_BMSR);
		log_info("mdio phy %d: id1=%#x id2=%#x bmsr=%#x",
				a, id1, id2, bmsr);
	}

	/* Detect the PHY and run autonegotiation; this also programs the
	 * negotiated speed into the MAC configuration via set_speed. */
	phy_detect(dev);
	log_info("phy_detect done, phy_id=%d", priv->phy_id);
	phy_autoneg(dev, 0);
	log_info("phy_autoneg done");

	eqos_set_macaddr_regs(priv, priv->macaddr);

	return eqos_hw_start(priv);
}

static int eqos_stop(struct net_device *dev) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;

	return eqos_hw_stop(priv);
}

static int eqos_set_macaddr(struct net_device *dev, const void *addr) {
	struct dwc_eqos_priv *priv = (struct dwc_eqos_priv *) dev->priv;

	memcpy(priv->macaddr, addr, ETH_ALEN);
	memcpy(dev->dev_addr, addr, ETH_ALEN);
	eqos_set_macaddr_regs(priv, priv->macaddr);

	return 0;
}

static const struct net_driver eqos_drv_ops = {
	.xmit        = eqos_xmit,
	.start       = eqos_open,
	.stop        = eqos_stop,
	.set_macaddr = eqos_set_macaddr,
	.mdio_read   = eqos_mdio_read,
	.mdio_write  = eqos_mdio_write,
	.set_phyid   = eqos_set_phyid,
	.set_speed   = eqos_set_speed,
};

static int eqos_parse_macaddr(const char *str, uint8_t *out) {
	int vals[ETH_ALEN];
	int i;

	if (sscanf(str, "%x:%x:%x:%x:%x:%x",
				&vals[0], &vals[1], &vals[2],
				&vals[3], &vals[4], &vals[5]) != ETH_ALEN) {
		return -EINVAL;
	}
	for (i = 0; i < ETH_ALEN; i++) {
		if ((vals[i] < 0) || (vals[i] > 0xff)) {
			return -EINVAL;
		}
		out[i] = (uint8_t) vals[i];
	}

	return 0;
}

int dwc_eqos_dev_init(const struct dwc_eqos_plat *plat) {
	struct dwc_eqos_priv *priv;
	struct net_device *nic;
	int res;

	if ((plat->idx < 0) || (plat->idx >= PORT_NUM)) {
		return -EINVAL;
	}
	priv = &eqos_priv[plat->idx];
	memset(priv, 0, sizeof(*priv));
	priv->plat = plat;
	priv->base_addr = plat->base_addr;
	priv->rx_ring = rx_rings[plat->idx];
	priv->tx_ring = tx_rings[plat->idx];

	if ((res = eqos_parse_macaddr(plat->mac_addr, priv->macaddr))) {
		log_error("bad mac_addr '%s'", plat->mac_addr);
		return res;
	}

	if (NULL == (nic = etherdev_alloc(0))) {
		return -ENOMEM;
	}
	nic->drv_ops = &eqos_drv_ops;
	nic->priv = priv;
	priv->dev = nic;

	log_info("%s: mac version %#x", plat->mac_addr,
			eqos_read(priv, DWC_EQOS_MAC_VERSION));

	/* SoC side: clocks, iomux, rgmii delays, then the PHY hard reset
	 * (which re-latches the RTL8211F strap we clear right after). */
	if (plat->soc_init == NULL) {
		log_error("no SoC glue hooked up");
		return -EINVAL;
	}
	res = plat->soc_init(plat);
	if (res) {
		return res;
	}

	log_info("%s: mac version after soc init %#x", plat->mac_addr,
			eqos_read(priv, DWC_EQOS_MAC_VERSION));

	if (plat->clear_rtl8211f_tx_delay) {
		eqos_phy_tx_delay_clear(priv);
	}

	res = eqos_hw_init(priv);
	if (res) {
		return res;
	}

	res = irq_attach(plat->irq_num, eqos_irq_handler, 0, nic,
			"dwc_eqos");
	if (res) {
		return res;
	}

	/* inetdev_register_dev (not bare netdev_register) creates the
	 * IPv4 interface object the stock stack's ifconfig/route/ping
	 * work through. */
	res = inetdev_register_dev(nic);
	if (res) {
		return res;
	}

	res = sys_timer_init_start_msec(&priv->rx_poll, SYS_TIMER_PERIODIC,
			100, eqos_rx_poll, priv);
	if (res) {
		return res;
	}

	log_info("%s at %#x irq %d mac %s", nic->name,
			(unsigned int) plat->base_addr, plat->irq_num,
			plat->mac_addr);

	return 0;
}
