/**
 * @file
 *
 * @date Sep 4, 2018
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <drivers/common/memory.h>

#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>
#include <net/mii.h>
#include <net/phy.h>

#include "dwc_gmac.h"
#include "dwc_desc.h"

#include <embox/unit.h>
#include <framework/mod/options.h>

extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);

EMBOX_UNIT_INIT(dwc_init);

#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM    OPTION_GET(NUMBER, irq_num)

#define RX_DESC_QUANTITY OPTION_GET(NUMBER, rx_desc_quantity)
#define TX_DESC_QUANTITY OPTION_GET(NUMBER, tx_desc_quantity)

struct dwc_priv {
	uint32_t                  base_addr;

	int                       phy_id;
	uint32_t                  mii_clk;
	uint8_t                   macaddr[ETH_ALEN];
	struct sk_buff           *rx_skb_pool[RX_DESC_QUANTITY];
	struct dma_extended_desc *rxdesc_ring_paddr;
	struct dma_extended_desc *txdesc_ring_paddr;
	int                       txdesc_id;
	int                       rxdesc_id;
};

static struct dwc_priv dwc_priv;

/* We need such big align to make sure that nothing else except buffers lies on
 * these MMU pages */
static uint8_t rx_buffers[RX_DESC_QUANTITY][0x1000] __attribute__((aligned(0x1000)));
static uint8_t tx_buffers[TX_DESC_QUANTITY][0x1000] __attribute__((aligned(0x1000)));

static inline uint32_t dwc_reg_read(struct dwc_priv *dwc_priv, uint32_t reg) {
	uint32_t res;

	assert(dwc_priv);

	res = REG32_LOAD(dwc_priv->base_addr + reg);
	log_debug("base %#X, reg %#X, res %#X", dwc_priv->base_addr, reg, res);

	return res;
}

static inline void dwc_reg_write(struct dwc_priv *dwc_priv, uint32_t reg, uint32_t value) {
	assert(dwc_priv);

	log_debug("base %#X, reg %#X, val %#X", dwc_priv->base_addr, reg, value);
	REG32_STORE(dwc_priv->base_addr + reg, value);
}

static void dwc_mdio_init(struct net_device *dev) {
	phy_detect(dev);
	phy_autoneg(dev, 0);
}

static void dwc_setup_rxfilter(struct dwc_priv *dwc_priv) {
	uint8_t *hwaddr;
	int ffval;
	int lo;
	int hi;

	ffval = 0;
	hwaddr = dwc_priv->macaddr;

	/*
	 * Set the multicast (group) filter hash.
	 */
	ffval = (DWC_GMAC_FRAME_FILTER_PM);

	/*
	 * Set the individual address filter hash.
	 */
	ffval |= (DWC_GMAC_FRAME_FILTER_PR);

	/*
	 * Set the primary address.
	 */
	lo = hwaddr[0] | (hwaddr[1] << 8) | (hwaddr[2] << 16) |
			(hwaddr[3] << 24);
	hi = hwaddr[4] | (hwaddr[5] << 8);
	dwc_reg_write(dwc_priv, DWC_GMAC_ADDRESS_LOW(0), lo);
	dwc_reg_write(dwc_priv, DWC_GMAC_ADDRESS_HIGH(0), hi);
	dwc_reg_write(dwc_priv, DWC_GMAC_FRAME_FILTER, ffval);

	log_debug("hwaddr = %x:%x:%x:%x:%x:%x", hwaddr[0], hwaddr[1], hwaddr[2],
			hwaddr[3], hwaddr[4], hwaddr[5]);
}

static void dwc_gmac_start(struct dwc_priv *dwc_priv) {
	uint32_t reg;

	dwc_setup_rxfilter(dwc_priv);

	/* Initialize DMA and enable transmitters */
	reg = dwc_reg_read(dwc_priv, DWC_DMA_OPERATION_MODE);
	reg |= (DWC_DMA_MODE_TSF | DWC_DMA_MODE_OSF | DWC_DMA_MODE_FUF);
	reg &= ~(DWC_DMA_MODE_RSF);
	reg |= (DWC_DMA_MODE_RTC_LEV32 << DWC_DMA_MODE_RTC_SHIFT);
	dwc_reg_write(dwc_priv, DWC_DMA_OPERATION_MODE, reg);

	dwc_reg_write(dwc_priv, DWC_DMA_INTERRUPT_ENABLE, DWC_DMA_INT_EN_DEFAULT);

	/* Start DMA */
	reg = dwc_reg_read(dwc_priv, DWC_DMA_OPERATION_MODE);
	reg |= (DWC_DMA_MODE_ST | DWC_DMA_MODE_SR);
	dwc_reg_write(dwc_priv, DWC_DMA_OPERATION_MODE, reg);

	/* Start transmit/receive engines */
	reg = dwc_reg_read(dwc_priv, DWC_GMAC_CONFIG);
	reg |= (DWC_GMAC_CONF_TE | DWC_GMAC_CONF_RE);
	dwc_reg_write(dwc_priv, DWC_GMAC_CONFIG, reg);
}

static int dwc_open(struct net_device *dev) {
	uint32_t version;
	struct dwc_priv *dwc_priv;

	assert(dev);

	dwc_priv = netdev_priv(dev);

	version = dwc_reg_read(dwc_priv, DWC_GMAC_VERSION);
	dwc_priv->mii_clk = (dwc_reg_read(dwc_priv, DWC_GMAC_MII_ADDR) >>
			DWC_GMAC_GMII_ADDRESS_CR_SHIFT) & DWC_GMAC_GMII_ADDRESS_CR_MASK;

	log_debug("version = %#X mmiclk %#X", version, dwc_priv->mii_clk);

	dwc_mdio_init(dev);

	dwc_gmac_start(dwc_priv);

	return 0;
}

static int dwc_setup_txdesc(struct dwc_priv *priv, int idx, uint32_t buff,
		int len) {
	struct dma_extended_desc *desc;

	assert(idx < TX_DESC_QUANTITY);

	desc = &priv->txdesc_ring_paddr[idx];

	desc->basic.des2 = buff;
	desc->basic.des1 = len & 0x1FFF;

	dcache_flush((void *) desc->basic.des2, len);

	desc->basic.des0 =
			ETDES0_SECOND_ADDRESS_CHAINED |
			ETDES0_FIRST_SEGMENT |
			ETDES0_LAST_SEGMENT |
			ETDES0_INTERRUPT;

	idx++;
	idx %= TX_DESC_QUANTITY;

	desc->basic.des3 = (uint32_t) &priv->txdesc_ring_paddr[idx];

	data_mem_barrier();
	desc->basic.des0 |= TDES0_OWN;
	data_mem_barrier();

	return idx;
}

static int dwc_desc_wait_trans(struct dwc_priv *priv, int idx) {
	struct dma_extended_desc *desc;

	desc = &priv->txdesc_ring_paddr[idx];
	do {
		/* TODO: add timeout.
		 * For some reason using usleep() here causes exception */
		data_mem_barrier();
	} while (desc->basic.des0 & ETDES0_OWN);

	return idx;
}

static int dwc_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct dwc_priv *priv;
	int cur_idx;

	assert(dev);
	assert(skb);

	priv = netdev_priv(dev);

	cur_idx = priv->txdesc_id;

	memcpy(&tx_buffers[cur_idx][0], skb_data_cast_in(skb->data), skb->len);

	priv->txdesc_id = dwc_setup_txdesc(priv, cur_idx,
			(uint32_t) &tx_buffers[cur_idx][0], skb->len);

	show_packet(&tx_buffers[cur_idx][0], skb->len, "tx");

	/* start transmit */
	dwc_reg_write(priv, DWC_DMA_TRANSMIT_POLL_DEMAND, 0x1);

	dwc_desc_wait_trans(priv, cur_idx);

	skb_free(skb);

	return 0;
}

static inline void dwc_get_macaddr(struct dwc_priv *dwc_priv, void *addr) {
	uint8_t *hwaddr;
	uint32_t lo;
	uint32_t hi;

	hwaddr = addr;

	lo = dwc_reg_read(dwc_priv, DWC_GMAC_ADDRESS_LOW(0));
	hi = dwc_reg_read(dwc_priv, DWC_GMAC_ADDRESS_HIGH(0)) & 0xffff;
	hwaddr[0] = (lo >>  0) & 0xff;
	hwaddr[1] = (lo >>  8) & 0xff;
	hwaddr[2] = (lo >> 16) & 0xff;
	hwaddr[3] = (lo >> 24) & 0xff;
	hwaddr[4] = (hi >>  0) & 0xff;
	hwaddr[5] = (hi >>  8) & 0xff;
	log_debug("hwaddr = %x:%x:%x:%x:%x:%x", hwaddr[0], hwaddr[1], hwaddr[2],
			hwaddr[3], hwaddr[4], hwaddr[5]);

}

static int dwc_set_macaddr(struct net_device *dev, const void *addr) {
	struct dwc_priv *dwc_priv;

	dwc_priv = netdev_priv(dev);

	memcpy(dwc_priv->macaddr, addr, sizeof(dwc_priv->macaddr));
	dwc_setup_rxfilter(dev->priv);

	return 0;
}

static int dwc_mdio_read(struct net_device *dev, uint8_t regAddr) {
	struct dwc_priv *dwc_priv;
	uint16_t mii;
	size_t cnt;
	int rv = 0;
	uint32_t reg;
	uint32_t phy;

	assert(dev);

	dwc_priv = netdev_priv(dev);

	reg = regAddr;
	phy = dwc_priv->phy_id;

	mii = ((phy & DWC_GMAC_GMII_ADDRESS_PA_MASK) << DWC_GMAC_GMII_ADDRESS_PA_SHIFT)
			| ((reg & DWC_GMAC_GMII_ADDRESS_GR_MASK) << DWC_GMAC_GMII_ADDRESS_GR_SHIFT)
			| (dwc_priv->mii_clk << DWC_GMAC_GMII_ADDRESS_CR_SHIFT)
			| DWC_GMAC_GMII_ADDRESS_GB; /* Busy flag */

	dwc_reg_write(dwc_priv, DWC_GMAC_MII_ADDR, mii);

	for (cnt = 0; cnt < 1000; cnt++) {
		if (!(dwc_reg_read(dwc_priv, DWC_GMAC_MII_ADDR) & DWC_GMAC_GMII_ADDRESS_GB)) {
			rv = dwc_reg_read(dwc_priv, DWC_GMAC_MII_DATA);
			break;
		}
		usleep(10 * USEC_PER_MSEC);
	}

	if (cnt == 1000) {
		log_error("Failed to read MDIO register");
	}

	return rv;
}

static int dwc_mdio_write(struct net_device *dev, uint8_t regAddr, uint16_t data) {
	struct dwc_priv *dwc_priv;
	uint32_t reg;
	uint32_t phy;
	uint16_t mii;
	size_t cnt;

	assert(dev);

	dwc_priv = netdev_priv(dev);

	reg = regAddr;
	phy = dwc_priv->phy_id;

	mii = ((phy & DWC_GMAC_GMII_ADDRESS_PA_MASK) << DWC_GMAC_GMII_ADDRESS_PA_SHIFT)
				| ((reg & DWC_GMAC_GMII_ADDRESS_GR_MASK) << DWC_GMAC_GMII_ADDRESS_GR_SHIFT)
				| (dwc_priv->mii_clk << DWC_GMAC_GMII_ADDRESS_CR_SHIFT)
				| DWC_GMAC_GMII_ADDRESS_GB | DWC_GMAC_GMII_ADDRESS_GW;

	dwc_reg_write(dwc_priv, DWC_GMAC_MII_DATA, data);
	dwc_reg_write(dwc_priv, DWC_GMAC_MII_ADDR, mii);

	for (cnt = 0; cnt < 1000; cnt++) {
		if (!(dwc_reg_read(dwc_priv, DWC_GMAC_MII_ADDR) & DWC_GMAC_GMII_ADDRESS_GB)) {
			break;
		}
		usleep(10 * USEC_PER_MSEC);
	}

	if (cnt == 1000) {
		log_error("Failed to write MDIO register");
	}

	return 0;
}

static void dwc_set_phyid(struct net_device *dev, uint8_t phyid) {
	struct dwc_priv *dwc_priv;

	assert(dev);

	dwc_priv = netdev_priv(dev);

	dwc_priv->phy_id = phyid;
}

static int dwc_set_speed(struct net_device *dev, int speed) {
	uint32_t reg;
	struct dwc_priv *dwc_priv;

	dwc_priv = netdev_priv(dev);

	reg = dwc_reg_read(dwc_priv, DWC_GMAC_CONFIG);

	reg |= DWC_GMAC_CONF_JD | DWC_GMAC_CONF_ACS | DWC_GMAC_CONF_BE;

	speed = net_to_mbps(speed);
	switch (speed) {
	case 1000:
		reg &= ~(DWC_GMAC_CONF_PS | DWC_GMAC_CONF_FES);
		break;
	case 100:
		reg |= DWC_GMAC_CONF_PS | DWC_GMAC_CONF_FES;
		break;
	default:
		log_error("Unsupported speed: %dmbps", speed);
		return -EINVAL;
	}

	reg |= DWC_GMAC_CONF_DM;
	dwc_reg_write(dwc_priv, DWC_GMAC_CONFIG, reg);
	return 0;
}

static const struct net_driver dwc_drv_ops = {
	.xmit        = dwc_xmit,
	.start       = dwc_open,
	.set_macaddr = dwc_set_macaddr,
	.mdio_read   = dwc_mdio_read,
	.mdio_write  = dwc_mdio_write,
	.set_phyid   = dwc_set_phyid,
	.set_speed   = dwc_set_speed
};

static uint32_t dwc_setup_rxdesc(struct dwc_priv *priv, int idx) {
	struct dma_extended_desc *desc;

	assert(idx < RX_DESC_QUANTITY);

	desc = &priv->rxdesc_ring_paddr[idx];

	memset(desc, 0, sizeof(*desc));
	data_mem_barrier();

	desc->basic.des2 = (uint32_t)&rx_buffers[idx][0];
	desc->basic.des1 = ERDES1_SECOND_ADDRESS_CHAINED | ETH_FRAME_LEN;

	idx++;
	idx %= RX_DESC_QUANTITY;
	desc->basic.des3 = (uint32_t)&priv->rxdesc_ring_paddr[idx];

	data_mem_barrier();
	desc->basic.des0 = RDES0_OWN;
	data_mem_barrier();

	return idx;
}

static inline int dwc_rxfinish_locked(struct net_device *dev_id) {
	int cur_desc;
	struct sk_buff *skb;
	struct dwc_priv *priv;
	struct dma_extended_desc *desc;
	int len;

	priv = netdev_priv(dev_id);

	for (;; ) {
		cur_desc = priv->rxdesc_id;

		data_mem_barrier();
		desc = &((struct dma_extended_desc *)priv->rxdesc_ring_paddr)[cur_desc];

		if (desc->basic.des0 & RDES0_OWN) {
			return 0;
		}

		len = (desc->basic.des0 >> 16) & 0x1FFF;
		if (NULL != (void *)desc->basic.des2) {
			dcache_inval((void *)(desc->basic.des2 & ~0xf), len);
		}

		skb = skb_alloc(len);
		if (!skb) {
			log_error("couldn't allocate skb");
			return -ENOMEM;
		}
		memcpy(skb_data_cast_in(skb->data), (void *)desc->basic.des2, len);

		skb->len = len - 4; /* without CRC */
		skb->dev = dev_id;

		show_packet(skb_data_cast_in(skb->data), skb->len, "rx");

		netif_rx(skb);

		priv->rxdesc_id = dwc_setup_rxdesc(priv, priv->rxdesc_id);
	}

	return 0;
}

static inline int dwc_txfinish_locked(struct net_device *dev_id) {
	/* do nothing. we have been already freed skb */
	return 0;
}

static int dwc_tx_ring_init(struct dwc_priv *priv) {
	memset(priv->txdesc_ring_paddr,
			0,
			sizeof(struct dma_extended_desc) * TX_DESC_QUANTITY);

	data_mem_barrier();
	return 0;
}

static int dwc_rx_ring_init(struct dwc_priv *priv) {
	int i;

	for (i = 0; i < RX_DESC_QUANTITY; i++) {
		dwc_setup_rxdesc(priv, i);
	}

	return 0;
}

static int setup_dma(struct dwc_priv *priv) {
	priv->txdesc_id = 0;
	priv->rxdesc_id = 0;

	dwc_rx_ring_init(priv);
	dwc_tx_ring_init(priv);

	return 0;
}

static int dwc_hw_init(struct dwc_priv *dwc_priv) {
	uint32_t reg;
	int i;

	/* Read MAC before reset */
	dwc_get_macaddr(dwc_priv, dwc_priv->macaddr);

	/* Disable all interrupts */
	dwc_reg_write(dwc_priv, DWC_DMA_INTERRUPT_ENABLE, 0);

	data_mem_barrier();
	/* Make sure RX/TX engines are not running */
	reg = dwc_reg_read(dwc_priv, DWC_GMAC_CONFIG);
	reg &= ~(DWC_GMAC_CONF_TE | DWC_GMAC_CONF_RE);
	dwc_reg_write(dwc_priv, DWC_GMAC_CONFIG, reg);

	data_mem_barrier();
	/* Reset */
	reg = dwc_reg_read(dwc_priv, DWC_DMA_BUS_MODE);
	reg |= (DWC_DMA_BUS_MODE_SWR);
	dwc_reg_write(dwc_priv, DWC_DMA_BUS_MODE, reg);
	for (i = 0; i < 1000; i++) {
		if ((dwc_reg_read(dwc_priv, DWC_DMA_BUS_MODE) & DWC_DMA_BUS_MODE_SWR) == 0) {
			break;
		}
		usleep(10 * USEC_PER_MSEC);
	}

	data_mem_barrier();
	if (i == 0 || i == 1000) {
		log_error("Can't reset DWC.");
		return (ENXIO);
	}

	data_mem_barrier();
	reg = dwc_reg_read(dwc_priv, DWC_DMA_BUS_MODE);
	reg |= (DWC_DMA_BUS_MODE_EIGHTXPBL);
	reg |= (DWC_DMA_BUS_MODE_PBL_BEATS_8 << DWC_DMA_BUS_MODE_PBL_SHIFT);
	dwc_reg_write(dwc_priv, DWC_DMA_BUS_MODE, reg);
	data_mem_barrier();

	/*
	 * DMA must be stop while changing descriptor list addresses.
	 */
	reg = dwc_reg_read(dwc_priv, DWC_DMA_OPERATION_MODE);
	reg &= ~(DWC_DMA_MODE_ST | DWC_DMA_MODE_SR); /*stop rx stop tx */
	dwc_reg_write(dwc_priv, DWC_DMA_OPERATION_MODE, reg);
	data_mem_barrier();

	while (dwc_reg_read(dwc_priv, DWC_DMA_STATUS) & DWC_DMA_STATUS_FSM_MASK) {}

	dwc_reg_read(dwc_priv, DWC_GMAC_INTERRUPT_STATUS);
	if (setup_dma(dwc_priv)) {
		return (ENXIO);
	}
	/* Setup addresses */
	data_mem_barrier();
	dwc_reg_write(dwc_priv, DWC_DMA_RX_DESCR_LIST_ADDR, (uint32_t)dwc_priv->rxdesc_ring_paddr);
	dwc_reg_write(dwc_priv, DWC_DMA_TX_DESCR_LIST_ADDR, (uint32_t)dwc_priv->txdesc_ring_paddr);

	data_mem_barrier();

	return 0;
}

static irq_return_t dwc_irq_handler(unsigned int irq_num, void *dev_id) {
	struct dwc_priv *priv;
	uint32_t reg;
	uint32_t tmp;
	ipl_t ipl;

	assert(dev_id);

	priv = netdev_priv(dev_id);

	reg = dwc_reg_read(priv, DWC_GMAC_INTERRUPT_STATUS);
	if (reg) {
		tmp = dwc_reg_read(priv, DWC_GMAC_SGMII_RGMII_SMII_CTRL_STATUS);
		log_debug("DWC_GMAC_INTERRUPT_STATUS %#X", tmp);
	}

	reg = dwc_reg_read(priv, DWC_GMAC_INTERRUPT_STATUS);
	ipl = ipl_save();
	{
		reg = dwc_reg_read(priv, DWC_DMA_STATUS);
		if (reg & DWC_DMA_STATUS_NIS) {
			if (reg & DWC_DMA_STATUS_RI) {
				dwc_rxfinish_locked(dev_id);
				log_debug("DWC_DMDWC_DMA_STATUS_RI");
			}

			if (reg & DWC_DMA_STATUS_TI) {
				dwc_txfinish_locked(dev_id);
				log_debug("DWC_DMDWC_DMA_STATUS_TI");
			}
		}

		if (reg & DWC_DMA_STATUS_AIS) {
			if (reg & DWC_DMA_STATUS_FBI) {
				/* Fatal bus error */
				log_error("Ethernet DMA error, restarting controller (%s)",
						((struct net_device *)dev_id)->name);
				dwc_hw_init(priv);
			}
		}

		dwc_reg_write(priv, DWC_DMA_STATUS, reg & DWC_DMA_STATUS_INTR_MASK);
	}
	ipl_restore(ipl);

	return IRQ_HANDLED;
}

static int dwc_init(void) {
	struct net_device *nic;
	uint32_t tmp;
	size_t rx_len = 0, tx_len = 0;

	if (NULL == (nic = etherdev_alloc(0))) {
		return -ENOMEM;
	}

	nic->drv_ops = &dwc_drv_ops;
	dwc_priv.base_addr = BASE_ADDR;

	rx_len = RX_DESC_QUANTITY * sizeof(struct dma_extended_desc);
	tx_len = TX_DESC_QUANTITY * sizeof(struct dma_extended_desc);

	dwc_priv.rxdesc_ring_paddr = periph_memory_alloc(rx_len);
	memset(dwc_priv.rxdesc_ring_paddr, 0, rx_len);
	/* TODO: this flush is neccessary even after we write to memory
	 * which was allocated as cached */
	dcache_flush(dwc_priv.rxdesc_ring_paddr, rx_len);

	dwc_priv.txdesc_ring_paddr = periph_memory_alloc(tx_len);
	memset(dwc_priv.txdesc_ring_paddr, 0, tx_len);
	dcache_flush(dwc_priv.txdesc_ring_paddr, tx_len);

	if (dwc_priv.rxdesc_ring_paddr == NULL ||
			dwc_priv.txdesc_ring_paddr == NULL) {
		etherdev_free(nic);
		return -ENOMEM;
	}

	nic->priv = &dwc_priv;

	dwc_hw_init(nic->priv);
	tmp = irq_attach(IRQ_NUM, dwc_irq_handler, 0, nic, "DWC_gmac");
	dwc_reg_write(nic->priv, DWC_DMA_INTERRUPT_ENABLE, DWC_DMA_INT_EN_DEFAULT);
	if (tmp) {
		return tmp;
	}

	return inetdev_register_dev(nic);
}

PERIPH_MEMORY_DEFINE(dwc, BASE_ADDR, 0x2000);
