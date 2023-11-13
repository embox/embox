/**
 * @file
 *
 * @date Jan 9, 2020
 *  @author Anton Bondarev
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <endian.h>

#include <util/log.h>
#include <util/math.h>

#include <asm/io.h>

#include <kernel/irq.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>

#include <net/l0/net_entry.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/l2/ethernet.h>
#include <net/inetdevice.h>
#include <net/mii.h>
#include <net/util/show_packet.h>

#include "l_e1000.h"
#include "dp83865_phy.h"

#include <framework/mod/options.h>



/** Number of receive descriptors per card. */
#define E1000_LOG_RX_BUFFERS       OPTION_GET(NUMBER,log_rx_desc_quantity)
/** Number of transmit descriptors per card. */
#define E1000_LOG_TX_BUFFERS       OPTION_GET(NUMBER,log_tx_desc_quantity)

#define E1000_CARD_QUANTITY        OPTION_GET(NUMBER,card_quantity)
#define E1000_PHY_ID               OPTION_GET(NUMBER,phy_id)

/*
 * Set the number of Tx and Rx buffers, using Log_2(# buffers).
 * Reasonable default values are 16 Tx buffers, and 256 Rx buffers.
 * That translates to 4 (16 == 2^^4) and 8 (256 == 2^^8).
 */
#define TX_RING_SIZE            (1 << (E1000_LOG_TX_BUFFERS))
#define TX_RING_MOD_MASK        (TX_RING_SIZE - 1)
#define TX_RING_LEN_BITS        ((E1000_LOG_TX_BUFFERS) << 12)

#define RX_RING_SIZE            (1 << (E1000_LOG_RX_BUFFERS))
#define RX_RING_MOD_MASK        (RX_RING_SIZE - 1)
#define RX_RING_LEN_BITS        ((E1000_LOG_RX_BUFFERS) << 4)


static const uint8_t l_base_mac_addr[6] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x02 };
static int card_number = 0;

struct l_e1000_dma_area {
	struct l_e1000_init_block init_block __attribute__((aligned(64)));
	struct l_e1000_rx_desc    rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));
	struct l_e1000_tx_desc    tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
};

struct mii_if {
	unsigned char full_duplex;
	unsigned char supports_gmii;
	unsigned char phy_id_mask;
	unsigned char reg_num_mask;
	unsigned char phy_id;
};

static struct l_e1000_dma_area e1000_dma_area[E1000_CARD_QUANTITY] __attribute__((aligned(64)));

struct l_e1000_priv {
	struct net_device *netdev;

	struct sk_buff *volatile rx_skbs[RX_RING_SIZE];
	struct sk_buff *volatile tx_skbs[TX_RING_SIZE];

	struct l_e1000_init_block *init_block;
	struct l_e1000_rx_desc    *rx_ring;
	struct l_e1000_tx_desc    *tx_ring;

	uintptr_t base_ioaddr; /* iomapped device regs */

	struct mii_if mii_if;
	int cur_rx;
};

static inline uint32_t e1000_read_e_csr(struct l_e1000_priv *ep) {
	assert(ep->base_ioaddr);
	return e2k_read32(ep->base_ioaddr + L_E1000_E_CSR);
}

static inline void e1000_write_e_csr(struct l_e1000_priv *ep, int val) {
	assert(ep->base_ioaddr);
	e2k_write32(val, ep->base_ioaddr + L_E1000_E_CSR);
}

static inline uint32_t e1000_read_mgio_csr(struct l_e1000_priv *ep) {
	assert(ep->base_ioaddr);
	return e2k_read32(ep->base_ioaddr + L_E1000_MGIO_CSR);
}

static inline void e1000_write_mgio_csr(struct l_e1000_priv *ep, int val) {
	assert(ep->base_ioaddr);
	e2k_write32(val, ep->base_ioaddr + L_E1000_MGIO_CSR);
}

static inline uint32_t e1000_read_mgio_data(struct l_e1000_priv *ep) {
	assert(ep->base_ioaddr);
	return e2k_read32(ep->base_ioaddr + L_E1000_MGIO_DATA);
}

static inline void e1000_write_mgio_data(struct l_e1000_priv *ep, int val) {
	assert(ep->base_ioaddr);
	e2k_write32(val, ep->base_ioaddr + L_E1000_MGIO_DATA);
}

static inline void e1000_write_e_base_addr(struct l_e1000_priv *ep, int val) {
	assert(ep->base_ioaddr);
	e2k_write32(val, ep->base_ioaddr + L_E1000_E_BASE_ADDR);
}

static inline void e1000_write_dma_base_addr(struct l_e1000_priv *ep, int val) {
	assert(ep->base_ioaddr);
	e2k_write32(val, ep->base_ioaddr + L_E1000_DMA_BASE_ADDR);
}

static inline uint32_t e1000_read_psf_csr(struct l_e1000_priv *ep) {
	assert(ep->base_ioaddr);
	return e2k_read32(ep->base_ioaddr + L_E1000_PSF_CSR);
}

static inline uint32_t e1000_read_psf_data(struct l_e1000_priv *ep) {
	assert(ep->base_ioaddr);
	return e2k_read32(ep->base_ioaddr + L_E1000_PSF_DATA);
}

static int e1000_mii_readreg(struct net_device *dev, int phy_id, int reg_num) {
	struct l_e1000_priv *ep = netdev_priv(dev);
	uint32_t rd;
	uint16_t val_out = 0;
	int i = 0;

	rd = 0;
	rd |= 0x2 << MGIO_CS_OFF;
	rd |= 0x1 << MGIO_ST_OF_F_OFF;
	rd |= 0x2 << MGIO_OP_CODE_OFF; /* Read */
	rd |= (phy_id  & 0x1f) << MGIO_PHY_AD_OFF;
	rd |= (reg_num & 0x1f) << MGIO_REG_AD_OFF;

	e1000_write_mgio_data(ep, rd);
	rd = 0;

	for (i = 0; i != 1000; i++) {
		if (e1000_read_mgio_csr(ep) & MGIO_CSR_RRDY) {
			rd = (uint16_t)e1000_read_mgio_data(ep);

			val_out = rd & 0xffff;
			log_debug("reg 0x%x >>> 0x%x", reg_num, val_out);
			return val_out;
		}
		usleep(100);
	}

	log_error("mdio_read: Unable to read from MGIO_DATA reg\n");

	return val_out;
}

static void e1000_mii_writereg(struct net_device *dev, int phy_id, int reg_num, int val) {
	struct l_e1000_priv *ep = netdev_priv(dev);
	uint32_t wr;
	int i = 0;

	wr = 0;
	wr |= 0x2 << MGIO_CS_OFF;
	wr |= 0x1 << MGIO_ST_OF_F_OFF;
	wr |= 0x1 << MGIO_OP_CODE_OFF; /* Write */
	wr |= (phy_id  & 0x1f) << MGIO_PHY_AD_OFF;
	wr |= (reg_num & 0x1f) << MGIO_REG_AD_OFF;
	wr |= val & 0xffff;

	log_debug("reg 0x%x <<< 0x%x", reg_num, val);
	e1000_write_mgio_data(ep, wr);

	for (i = 0; i != 1000; i++) {
		if (e1000_read_mgio_csr(ep) & MGIO_CSR_RRDY) {
			return;
		}
		usleep(100);
	}
	log_error("Unable to write MGIO_DATA reg: val = 0x%x", wr);
	return;
}

static inline void
e1000_phy_mmd_write(void *eth1000_ptr, uint8_t mmd_dev, uint16_t mmd_reg, uint16_t val) {
	e1000_mii_writereg(eth1000_ptr, 0, 0xD, mmd_dev & 0x1F);
	e1000_mii_writereg(eth1000_ptr, 0, 0xE, mmd_reg);
	e1000_mii_writereg(eth1000_ptr, 0, 0xD, 0x01 << 14 | mmd_dev & 0x1F);
	e1000_mii_writereg(eth1000_ptr, 0, 0xE, val);
}

#define SPEED_10   10
#define SPEED_100  100
#define SPEED_1000 1000

static void l_e1000_set_phy_mode(struct net_device *dev) {
	static const int assigned_speed = SPEED_1000;
	struct l_e1000_priv *ep = netdev_priv(dev);
	unsigned int val;
	unsigned int lpa;
	unsigned int advertise;
	unsigned int media;
	unsigned int lpa2;
	unsigned int advertise2;
	int speed = SPEED_1000;

	lpa = e1000_mii_readreg(dev, ep->mii_if.phy_id, MII_LPA);
	advertise = e1000_mii_readreg(dev, ep->mii_if.phy_id, MII_ADVERTISE);
	log_debug("MII lpa 0x%x advertise 0x%x", lpa, advertise);
	if (ep->mii_if.supports_gmii) {
		lpa2 = e1000_mii_readreg(dev, ep->mii_if.phy_id, MII_STAT1000);
		advertise2 = e1000_mii_readreg(dev, ep->mii_if.phy_id, MII_CTRL1000);
		log_debug(" GMII status 0x%x control 0x%x", lpa2, advertise2);
		if ((advertise2 & (ADVERTISE_1000HALF | ADVERTISE_1000FULL)) &&
				(lpa2 & (LPA_1000FULL | LPA_1000HALF))) {
			speed = SPEED_1000;
		}
	}
	if (speed == 0) {
		media = mii_nway_result(lpa & advertise);
		if (media & (ADVERTISE_100FULL | ADVERTISE_100HALF)) {
			speed = SPEED_100;
		} else {
			speed = SPEED_10;
		}
	}
	if (speed != assigned_speed) {
		log_debug("decrease speed to %d due to module param", SPEED_1000);
		speed = assigned_speed;
		if (ep->mii_if.supports_gmii && (speed < SPEED_1000)) {
				lpa2 &= ~(LPA_1000FULL | LPA_1000HALF);
				e1000_mii_writereg(dev, ep->mii_if.phy_id, MII_STAT1000, lpa2);
				advertise2 &= ~(ADVERTISE_1000HALF | ADVERTISE_1000FULL);
				e1000_mii_writereg(dev, ep->mii_if.phy_id, MII_CTRL1000, advertise2);
		}
		if (speed == SPEED_10) {
			lpa &= ~(LPA_100FULL | LPA_100BASE4 | LPA_100HALF);
			e1000_mii_writereg(dev, ep->mii_if.phy_id, MII_LPA, lpa);

			advertise &= ~(ADVERTISE_1000XFULL |
				ADVERTISE_1000XHALF |
				ADVERTISE_1000XPSE_ASYM |
				ADVERTISE_1000XPAUSE);
			advertise |= (ADVERTISE_10FULL | ADVERTISE_10HALF);
			e1000_mii_writereg(dev, ep->mii_if.phy_id, MII_ADVERTISE, advertise);
		}
	}
	log_debug("e1000_set_phy_mode will set %d Mbits %s-duplex mode",
		speed, (ep->mii_if.full_duplex) ? "full" : "half");
	val = e1000_read_mgio_csr(ep);
	log_debug("mgio_csr before set : 0x%x", val);
	val |= MGIO_CSR_HARD;
	e1000_write_mgio_csr(ep, val);
	log_debug("mgio_csr after writing HARD = 1 in : 0x%x", e1000_read_mgio_csr(ep));
	val &= ~(MGIO_CSR_FETH | MGIO_CSR_GETH | MGIO_CSR_FDUP);
	if (ep->mii_if.full_duplex) {
		val |= MGIO_CSR_FDUP;
	}
	if (speed >= SPEED_1000) {
		val |= MGIO_CSR_GETH;
	} else if (speed >= SPEED_100) {
		val |= MGIO_CSR_FETH;
	}

	e1000_write_mgio_csr(ep, val);
	log_debug("mgio_csr after setting %d Mbits %s-duplex mode : 0x%x",
		speed, (ep->mii_if.full_duplex) ? "full" : "half", e1000_read_mgio_csr(ep));
}

static struct sk_buff *l_e1000_new_rx_buff(struct l_e1000_priv *ep, int i) {
	struct sk_buff *skb;
	struct l_e1000_rx_desc *rdesc;

	log_debug("i %d", i);
	skb = skb_alloc(ETH_FRAME_LEN + ETH_FCS_LEN);
	if (skb == NULL) {
		log_error("couldn't skb_alloc(i %d)", i);
		return NULL;
	}
	log_debug("skb = %p", skb);

	rdesc = &ep->rx_ring[i];
	rdesc->base = htole32((uintptr_t)skb_get_data_pointner(skb->data));
	rdesc->buf_length = htole16(-(ETH_FRAME_LEN + ETH_FCS_LEN));
	wmb();
	rdesc->status = htole16(RD_OWN);
	wmb();
	log_debug("rxdesc[%d](base=0x%x,len=%d)", i,
			le32toh(rdesc->base), le32toh(rdesc->buf_length));

	return skb;
}

/* Initialize the E1000 Rx and Tx rings. */
static int e1000_init_block(struct net_device *dev) {
	struct l_e1000_priv *ep = netdev_priv(dev);
	uint32_t init_block_addr_part;
	int i;

	ep->cur_rx = 0;

	for (i = 0; i < RX_RING_SIZE; i++) {
		ep->rx_skbs[i] = l_e1000_new_rx_buff(ep, i);
		if (NULL == ep->rx_skbs[i]) {
			log_error("Coudn't alloc sk_buff");
			return -ENOMEM;
		}
	}

	for (i = 0; i < TX_RING_SIZE; i++) {
		ep->tx_skbs[i] = NULL;
	}

	ep->tx_ring->status = 0;
	wmb();
	ep->tx_ring->base = 0;
	ep->tx_ring->buf_length = 0;

	/* Setup init block */
	/************************************************************************/
	ep->init_block->mode = htole16(FULL|PROM);
	ep->init_block->laddrf = 0x0;
	for (i = 0; i < 6; i++) {
		ep->init_block->paddr[i] = dev->dev_addr[i];
	}

	ep->init_block->rdra = htole32((uint32_t)((uintptr_t)(&ep->rx_ring[0])));
	ep->init_block->rdra |= htole32(E1000_LOG_RX_BUFFERS);
	ep->init_block->tdra = htole32((uint32_t)((uintptr_t)(&ep->tx_ring[0])));
	ep->init_block->tdra |= htole32(E1000_LOG_TX_BUFFERS);
	log_debug("Rx Desc Ring DMA Addr 0x%x", le32toh(ep->init_block->rdra));
	log_debug("Tx Desc Ring DMA Addr 0x%x", le32toh(ep->init_block->tdra));
	/***********************************************************************/
	/* low 32 bits */
	init_block_addr_part = (uint32_t)((uintptr_t)ep->init_block & 0xffffffff);
	e1000_write_e_base_addr(ep, init_block_addr_part);
	log_debug("Init Block Low  DMA addr: 0x%x", init_block_addr_part);
	/* high 32 bits */
	init_block_addr_part = (uint32_t)(((uintptr_t)(ep->init_block) >> 32) & 0xffffffff);
	e1000_write_dma_base_addr(ep, init_block_addr_part);
	log_debug("Init Block High DMA addr: 0x%x", init_block_addr_part);
	/************************************************************************/

	return 0;
}

static int l_e1000_reset(struct net_device *dev) {
	struct l_e1000_priv *ep = netdev_priv(dev);
	int i;

	/* Reset the PCNET32 */
	e1000_write_e_csr(ep, E_CSR_STOP);
	/* wait for stop */
	for (i = 0; i < 1000; i++) {
		if (e1000_read_e_csr(ep) & E_CSR_STOP) {
			break;
		}
	}

	e1000_init_block(dev);

	e1000_write_e_csr(ep, E_CSR_INIT);
	l_e1000_set_phy_mode(dev);

	sleep(5);

	i = 0;
	while (i++ < 1000) {
		if (e1000_read_e_csr(ep) & E_CSR_IDON) {
			break;
		}
	}

	log_debug("e_csr register after initialization: 0x%x, must be 0x%x",
			e1000_read_e_csr(ep), (E_CSR_IDON | E_CSR_INTR | E_CSR_INIT));

	e1000_write_e_csr(ep, E_CSR_IDON);
	log_debug("e_csr register after clear IDON bit: 0x%x, must be 0x%x",
			e1000_read_e_csr(ep), (E_CSR_INIT));

	e1000_write_e_csr(ep, E_CSR_INEA | E_CSR_STRT);
	log_debug("e_csr register after setting STRT bit: 0x%x, must be 0x%x",
			e1000_read_e_csr(ep),
			(E_CSR_INEA | E_CSR_RXON | E_CSR_TXON | E_CSR_STRT | E_CSR_INIT));

	return 0;
}

static int l_e1000_hw_init(struct pci_slot_dev *pci_dev, struct net_device *dev, int number) {
	struct l_e1000_priv *ep;
	struct l_e1000_dma_area *m;

	int fdx, mii, gmii;
	uint32_t val = 0;
	int i;

	ep = netdev_priv(dev);

	m = &e1000_dma_area[number];

	ep->init_block = &m->init_block;
	ep->tx_ring = &m->tx_ring[0];
	ep->rx_ring = &m->rx_ring[0];

	ep->base_ioaddr = dev->base_addr;
	ep->netdev = dev;

#if 0 /*soft reset */
	/* Setup STOP bit; Force e1000 resetting  */
	{
		unsigned int soft_reset;

		e1000_write_e_csr(ep, E_CSR_STOP);
		/* PHY Resetting */
		soft_reset = 0;
		soft_reset |= (L_E1000_RSET_POLARITY | MGIO_CSR_SRST);
		e1000_write_mgio_csr(ep, soft_reset); /* startup software reset */
		soft_reset = e1000_read_mgio_csr(ep);
		soft_reset &= ~(MGIO_CSR_SRST);
		e1000_write_mgio_csr(ep, soft_reset); /* stop software reset */
	}
#endif /*soft reset */

	for (i = 0; i < 6; i++) {
		dev->dev_addr[i] = l_base_mac_addr[i];
	}
	dev->dev_addr[5] = l_base_mac_addr[5] + number;

	fdx = 1; mii = 1; gmii = 1;
	ep->mii_if.full_duplex = fdx;
	ep->mii_if.supports_gmii = gmii;
	ep->mii_if.phy_id_mask = 0x1f;
	ep->mii_if.reg_num_mask = 0x1f;
	ep->mii_if.phy_id = E1000_PHY_ID;

	/* Setup PHY MII/GMII enable */
/*	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, PHY_AUX_CTRL);
	log_info("PHY reg # 0x12 (AUX_CTRL) : after reset : 0x%x\n", val);
	val &= ~(RGMII_EN_1 | RGMII_EN_0);
	e1000_mii_writereg(dev, ep->mii_if.phy_id, PHY_AUX_CTRL, val);
*/
	/* Setup PHY 10/100/1000 Link on 10M Link */
/*	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, PHY_LED_CTRL);
	log_info("PHY reg # 0x13 (LED_CTRL) : after reset : 0x%x\n", val);
	val |= RED_LEN_EN;
	e1000_mii_writereg(dev, ep->mii_if.phy_id, PHY_LED_CTRL, val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, PHY_LED_CTRL);
	log_info("PHY reg # 0x13 (LED_CTRL) : after led is : 0x%x\n", val);
*/
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, 0);
	log_info("PHY reg # 0x0  after reset : 0x%x\n", val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, 0x1);
	log_info("PHY reg # 0x1  after reset : 0x%x\n", val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, 0x1);
	log_info("PHY reg # 0x1  after reset : 0x%x\n", val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, 0x10);
	log_info("PHY reg # 0x10  after reset : 0x%x\n", val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, 0x11);
	log_info("PHY reg # 0x11  after reset : 0x%x\n", val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, 0x14);
	log_info("PHY reg # 0x14  after reset : 0x%x\n", val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, 0x15);
	log_info("PHY reg # 0x15  after reset : 0x%x\n", val);
/*
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, PHY_BIST_CFG2);
	log_info("PHY reg # 0x1a (BIST_CFG2): 0x%x\n", val);
	val |= LINK_SEL;
	e1000_mii_writereg(dev, ep->mii_if.phy_id, PHY_BIST_CFG2, val);
	val = e1000_mii_readreg(dev, ep->mii_if.phy_id, PHY_BIST_CFG2);
	log_info(" read mgio csr #0x04 (BIST_CFG2) : 0x%x\n", val);
*/
/* Microchip phy special regs */
#if 0
	e1000_phy_mmd_write(dev, 0x0, 0x4, 0x0006);
	e1000_phy_mmd_write(dev, 0x0, 0x3, 0x1A80);
#endif
/* End Microchip phy special regs */
	/* move e1000 link status select to default 0 link */
/* Marvel specific */
#if 0
	log_info("move e1000 link status select to default 0 link \n");
	val = e1000_read_mgio_csr(ep);
	val &= ~MGIO_CSR_LSTS;
	val |= MGIO_CSR_SLSP;
	e1000_write_mgio_csr(ep, val);
	log_info(" write mgio csr #0x04 (BIST_CFG2): 0x%x\n", val);
#endif /* End Marvel specific */

	return 0;
}

static int l_e1000_open(struct net_device *dev) {
	log_debug("mac addr: %x:%x:%x:%x:%x:%x\n",
			dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
			dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
	l_e1000_reset(dev);
	return 0;
}

static int l_e1000_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct l_e1000_priv *ep = netdev_priv(dev);
	uint16_t status;
	int len;

	len = max(skb->len, ETH_ZLEN);

	show_packet(skb_get_data_pointner(skb->data), len, "transmit");

	/* wait until buff is nor free */

	while(ep->tx_skbs[0] != NULL) {
	}

	ep->tx_ring->base = (uint32_t)(uintptr_t)skb_get_data_pointner(skb->data);
	ep->tx_ring->buf_length = htole16(-len);
	ep->tx_ring->misc = 0x00000000;
	status = TD_OWN | TD_ENP | TD_STP;
	ep->tx_ring->status = htole16(status);
	wmb();
	e1000_write_e_csr(ep, E_CSR_INEA | E_CSR_TDMD);

	/* TODO now we send only one packet */
	ep->tx_skbs[0] = skb;

	return 0;
}

static int l_e1000_set_mac_address(struct net_device *dev, const void *addr) {
	memcpy(&dev->dev_addr[0], addr, sizeof(dev->dev_addr));
	log_debug("mac addr: %x:%x:%x:%x:%x:%x\n",
			dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
			dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
	return 0;
}

static int l_e1000_stop(struct net_device *dev) {
	return 0;
}

static const struct net_driver l_e1000_drv_ops = {
	.xmit = l_e1000_xmit,
	.start = l_e1000_open,
	.stop = l_e1000_stop,
	.set_macaddr = l_e1000_set_mac_address
};

static int l_e1000_rx(struct l_e1000_priv *ep) {
	uint8_t * buf = NULL;
	int msg_len;
	int entry;

	entry  = ep->cur_rx & RX_RING_MOD_MASK;

	/* If we own the next entry, it's a new packet. Send it up. */
	while ((le16toh(ep->rx_ring[entry].status)) >= 0) {
		struct sk_buff *skb;
		int status;

		ep->cur_rx++;

		status = le16toh(ep->rx_ring[entry].status);
		skb = ep->rx_skbs[entry];

		if ((status & 0xff00) != (RD_ENP|RD_STP)) {/* There was an error. */
			msg_len = 0;
			if (status & RD_ENP) {
				log_debug(" ENP");
			} /* No detailed rx_errors counter to increment at the */
			/* end of a packet.*/
			if (status & RD_FRAM) {
				log_debug(" FRAM");
			}
			if (status & RD_OFLO) {
				log_debug(" OFLO ");
			}
			if (status & RD_CRC) {
				log_debug(" CRC ");
			}
			if (status & RD_BUFF) {
				log_debug(" BUFF ");
			}
			skb_free(skb);
		} else {
			buf = (void *)(uintptr_t)le32toh(ep->rx_ring[entry].base);
			msg_len =  (le16toh(ep->rx_ring[entry].msg_length) & 0xfff) - ETH_FCS_LEN;
			show_packet(buf, msg_len, " --- BUF received packet print:");
			skb->dev = ep->netdev;
			skb->len = msg_len;

			netif_rx(skb);
		}

		ep->rx_skbs[entry] = l_e1000_new_rx_buff(ep, entry);

		entry  = ep->cur_rx & RX_RING_MOD_MASK;
	}
	return 0;
}

static irq_return_t l_e1000_interrupt(unsigned int irq_num, void *dev_id) {
	struct net_device *dev = dev_id;
	struct l_e1000_priv *ep;
	uint16_t csr0;//, csr_ack;

	ep = netdev_priv(dev);
	csr0 = e1000_read_e_csr(ep);
	if (!(csr0 & E_CSR_INTR)) {
		return IRQ_NONE; /* Not our interrupt */
	}

	log_debug("irq csr0(%x)", csr0);

	csr0 &= (E_CSR_BABL | E_CSR_CERR | E_CSR_MISS | E_CSR_MERR | E_CSR_RINT | E_CSR_TINT);
	e1000_write_e_csr(ep, csr0 | E_CSR_IDON);

	if (csr0 & E_CSR_TINT) {
		int status = le16toh(ep->tx_ring->status);
		if (status & TD_ERR) {
			int err_status = le32toh(ep->tx_ring->misc);
			log_error("Tx error status=%04x err_status=%08x", status, err_status);
		}

		ep->tx_ring->status = 0;
		assert(ep->tx_skbs[0]);
		skb_free(ep->tx_skbs[0]);

		ep->tx_skbs[0] = NULL;
	}

	if (csr0 & E_CSR_MERR) {
		log_debug("irq MERR");
	}
	if (csr0 & E_CSR_BABL) {
		log_debug("irq BABL");
	}

	if (csr0 & E_CSR_MISS) {
		/* The device did't set RINT in case of buffers are full
		* and MISS is set. So we do this manually.
		*/
		csr0 |= E_CSR_RINT;
		log_debug("irq BABL");
	}

	if (csr0 & E_CSR_RINT) {
		l_e1000_rx(ep);
	}

	e1000_write_e_csr(ep, E_CSR_INEA);

	return IRQ_HANDLED;
}

static int l_e1000_init(struct pci_slot_dev *pci_dev) {
	int res;
	struct net_device *nic;
	struct l_e1000_priv *nic_priv;
	int number;

	if (card_number == E1000_CARD_QUANTITY) {
		return 0;
	}
	number = card_number;
	card_number++;

	nic = (struct net_device *) etherdev_alloc(sizeof(struct l_e1000_priv));
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &l_e1000_drv_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = (uintptr_t) mmap_device_memory(
			(void *) (uintptr_t) (pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK),
			L_E1000_TOTAL_PCI_IO_SIZE,
			PROT_WRITE | PROT_READ | PROT_NOCACHE,
			MAP_FIXED,
			pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK);

	log_debug("bar (%p)\n base (%x) \n poor bar(%x)",
			(void *) (uintptr_t) (pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK),
			nic->base_addr,
			(uint64_t)(pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK));
			
	nic_priv = netdev_priv(nic);
	memset(nic_priv, 0, sizeof(*nic_priv));

	res = irq_attach(pci_dev->irq, l_e1000_interrupt, IF_SHARESUP, nic, "l_e1000");
	if (res < 0) {
		return res;
	}

	pci_set_master(pci_dev);
	l_e1000_hw_init(pci_dev, nic, number);

	return inetdev_register_dev(nic);
}

static const struct pci_id l_e1000_id_table[] = {
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_MCST_ELBRUS_E1000 },
	{ 0x1FFF, 0x8016},
};

PCI_DRIVER_TABLE("l_e1000", l_e1000_init, l_e1000_id_table);
