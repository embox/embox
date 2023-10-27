/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 09.09.23
 */
#include <endian.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <embox/unit.h>
#include <hal/cache.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/mii.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>
#include <util/field.h>
#include <util/log.h>

#include "greth.h"

#define GRETH_RXBD_CNT 4
#define GRETH_TXBD_CNT 4

EMBOX_UNIT_INIT(greth_init);

static void greth_bd_save(struct greth_priv *greth, struct greth_bd *dst,
    const struct greth_bd *src) {
	REG32_STORE(&dst->addr, htobe32(src->addr));
	REG32_STORE(&dst->stat, htobe32(src->stat));

	dcache_flush(dst, sizeof(struct greth_bd));
}

static void greth_bd_load(struct greth_priv *greth, struct greth_bd *dst,
    const struct greth_bd *src) {
	dcache_inval(src, sizeof(struct greth_bd));

	dst->addr = be32toh(REG32_LOAD(&src->addr));
	dst->stat = be32toh(REG32_LOAD(&src->stat));
}

static void greth_wait_bit_reset(void *reg, uint32_t mask) {
	int cnt;

	for (cnt = 0; cnt < 1000; cnt++) {
		if (!(REG32_LOAD(reg) & mask)) {
			break;
		}
		usleep(10 * USEC_PER_MSEC);
	}

	if (cnt == 1000) {
		log_error("Timeout!");
	}
}

static int greth_mdio_read(struct net_device *dev, uint8_t reg) {
	struct greth_priv *greth;
	uint32_t ctrl;

	greth = netdev_priv(dev);

	ctrl = GRETH_MDIO_RD;
	ctrl = FIELD_SET(ctrl, GRETH_MDIO_RA, reg);
	ctrl = FIELD_SET(ctrl, GRETH_MDIO_PA, greth->phy_id);

	greth_wait_bit_reset(&GRETH_REGS(dev)->mdio, GRETH_MDIO_BU);
	REG32_STORE(&GRETH_REGS(dev)->mdio, ctrl);

	greth_wait_bit_reset(&GRETH_REGS(dev)->mdio, GRETH_MDIO_BU);
	ctrl = REG32_LOAD(&GRETH_REGS(dev)->mdio);

	if (ctrl & GRETH_MDIO_NV) {
		log_error("Not valid data");
	}

	return FIELD_GET(ctrl, GRETH_MDIO_DT);
}

static int greth_mdio_write(struct net_device *dev, uint8_t reg,
    uint16_t data) {
	struct greth_priv *greth;
	uint32_t ctrl;

	greth = netdev_priv(dev);

	ctrl = GRETH_MDIO_WR;
	ctrl = FIELD_SET(ctrl, GRETH_MDIO_DT, data);
	ctrl = FIELD_SET(ctrl, GRETH_MDIO_RA, reg);
	ctrl = FIELD_SET(ctrl, GRETH_MDIO_PA, greth->phy_id);

	log_debug("0x%08X < 0x%08X [p:%d a:%d d:0x%04X]",
	    (uint32_t)&GRETH_REGS(dev)->mdio, ctrl, greth->phy_id, reg, data);

	greth_wait_bit_reset(&GRETH_REGS(dev)->mdio, GRETH_MDIO_BU);
	REG32_STORE(&GRETH_REGS(dev)->mdio, ctrl);
	greth_wait_bit_reset(&GRETH_REGS(dev)->mdio, GRETH_MDIO_BU);

	return 0;
}

static void greth_set_phyid(struct net_device *dev, uint8_t phyid) {
	struct greth_priv *greth;

	greth = netdev_priv(dev);
	greth->phy_id = phyid;
}

static int greth_set_macaddr(struct net_device *dev, const void *addr) {
	const uint8_t *ptr;
	uint32_t mac[2];
	int i;

	ptr = addr;

	mac[0] = (ptr[0] << 8) | ptr[1];
	mac[1] = (ptr[2] << 24) | (ptr[3] << 16) | (ptr[4] << 8) | ptr[5];

	REG32_STORE(&GRETH_REGS(dev)->mac_msb, mac[0]);
	REG32_STORE(&GRETH_REGS(dev)->mac_lsb, mac[1]);

	for (i = 0; i < ETH_ALEN; i++) {
		dev->dev_addr[i] = ptr[i];
	}

	return 0;
}

static int greth_set_speed(struct net_device *dev, int speed) {
	uint32_t ctrl;

	log_debug("GRETH mode:");

	ctrl = REG32_LOAD(&GRETH_REGS(dev)->control);

	if (net_is_fullduplex(speed)) {
		log_debug("full duplex");
		ctrl |= GRETH_CTRL_FD;
	}
	else {
		log_debug("half duplex");
		ctrl &= ~GRETH_CTRL_FD;
	}

	speed = net_to_mbps(speed);
	log_debug("%u Mbps", speed);

	switch (speed) {
	case 1000:
		ctrl |= GRETH_CTRL_GB;
		break;
	case 100:
		ctrl &= ~GRETH_CTRL_GB;
		ctrl |= GRETH_CTRL_SP;
		break;
	case 10:
		ctrl &= ~GRETH_CTRL_GB;
		ctrl &= ~GRETH_CTRL_SP;
		break;
	}

	REG32_STORE(&GRETH_REGS(dev)->control, ctrl);

	return 0;
}

static int greth_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct greth_priv *greth;
	struct greth_bd bd;
	void *data;
	size_t len;

	log_debug("Trying to send a packet");

	greth = netdev_priv(dev);
	data = skb_data_cast_in(skb->data);
	len = skb->len;

	/* Clear tx status */
	REG32_STORE(&GRETH_REGS(dev)->status, GRETH_STAT_TE | GRETH_STAT_TI);

	/* Make sure packet data is not cached */
	dcache_flush(data, len);

	/* Set up descriptor to wrap around to it self */
	bd.addr = (uint32_t)data;
	bd.stat = GRETH_BD_EN | GRETH_BD_WR;
	bd.stat = FIELD_SET(bd.stat, GRETH_BD_LE, len);

	/* Store current transmit descriptor */
	greth_bd_save(greth, greth->txbd_base, &bd);

	/* Enable transmitter */
	REG32_ORIN(&GRETH_REGS(dev)->control, GRETH_CTRL_TE);
	/* Wait for data to be sent */
	do {
		greth_bd_load(greth, &bd, greth->txbd_base);
	} while (bd.stat & GRETH_BD_EN);

	show_packet(data, len, "transmitted");

	/* Was the packet transmitted succesfully? */
	if (bd.stat & GRETH_TXBD_UE) {
		log_error("Underrun error");
	}
	if (bd.stat & GRETH_TXBD_AL) {
		log_error("Attempt limit error");
	}

	skb_free(skb);

	return 0;
}

static const struct net_driver greth_ops = {
    .xmit = greth_xmit,
    .set_macaddr = greth_set_macaddr,
    .mdio_read = greth_mdio_read,
    .mdio_write = greth_mdio_write,
    .set_phyid = greth_set_phyid,
    .set_speed = greth_set_speed,
};

static irq_return_t greth_irq_handler(unsigned int irq_num, void *_dev) {
	struct greth_priv *greth;
	struct net_device *dev;
	struct sk_buff *skb;
	void *data;
	size_t len;
	struct greth_bd bd;
	unsigned int bd_nr;

	log_debug("Trying to receive a packet");

	dev = _dev;
	greth = netdev_priv(dev);

	/* Load current receive descriptor */
	greth_bd_load(greth, &bd, greth->rxbd_base + greth->rxbd_curr);

	/* Stop if no more packets received */
	if (bd.stat & GRETH_BD_EN) {
		goto out;
	}

	/* Clear rx status */
	REG32_STORE(&GRETH_REGS(dev)->status, GRETH_STAT_RE | GRETH_STAT_RI);

	data = (void *)bd.addr;
	len = FIELD_GET(bd.stat, GRETH_BD_LE);
	bd_nr = greth->rxbd_curr;
	skb = greth->rxbuf_base[bd_nr];

	/* Make sure we're not reading old packet data */
	dcache_inval(data, len);

	show_packet(data, len, "received");

	/* Was the packet received succesfully? */
	if (bd.stat & GRETH_RXBD_AE) {
		log_error("Alignment error");
	}
	if (bd.stat & GRETH_RXBD_FT) {
		log_error("Frame too long error");
	}
	if (bd.stat & GRETH_RXBD_CE) {
		log_error("CRC error");
	}
	if (bd.stat & GRETH_RXBD_OE) {
		log_error("Overrun error");
	}
	if (bd.stat & GRETH_RXBD_LE) {
		log_error("Length error");
	}

	/* Pass packet on to network subsystem */
	skb->dev = dev;
	skb->len = len;
	netif_rx(skb);

	/* Reenable descriptor */
	skb = greth->rxbuf_base[bd_nr] = skb_alloc(ETH_FRAME_LEN);

	bd.stat = GRETH_BD_EN | GRETH_BD_IE;
	bd.addr = (uint32_t)skb_data_cast_in(skb->data);

	if (++greth->rxbd_curr == GRETH_RXBD_CNT) {
		bd.stat |= GRETH_BD_WR;
		greth->rxbd_curr = 0;
	}

	greth_bd_save(greth, greth->rxbd_base + bd_nr, &bd);

	/* Enable receiver */
	REG32_ORIN(&GRETH_REGS(dev)->control, GRETH_CTRL_RE | GRETH_CTRL_RI);

out:
	return IRQ_HANDLED;
}

static void greth_init_phy(struct net_device *dev) {
	struct greth_regs *regs;
	int net_speed;
	int gbit_mac;
	int ctrl;
	int stat;
	int tmp1;
	int tmp2;
	int i;

	regs = GRETH_REGS(dev);
	net_speed = NET_10HALF;

	log_debug("Resetting PHY");

	/* Determine PHY address */
	for (i = 0; i < 32; i++) {
		greth_set_phyid(dev, i);
		if (greth_mdio_read(dev, MII_PHYSID1) != 0xffff) {
			log_debug("PHY address = %i", i);
			break;
		}
	}
	if (i == 32) {
		log_error("PHY address is not determined");
	}

	/* Reset PHY */
	while ((ctrl = greth_mdio_read(dev, MII_BMCR)) & BMCR_RESET) {}
	greth_mdio_write(dev, MII_BMCR, ctrl | BMCR_RESET);

	/* Wait for PHY reset completion */
	while ((ctrl = greth_mdio_read(dev, MII_BMCR)) & BMCR_RESET) {}

	/* Check if MAC is gigabit capable */
	gbit_mac = REG32_LOAD(&regs->control) & GRETH_CTRL_GA;

	/* Check if PHY is autoneg capable */
	if (!(ctrl & BMCR_ANENABLE)) {
		goto auto_neg_done;
	}

	/* Wait for auto negotiation to complete */
	while ((stat = greth_mdio_read(dev, MII_BMSR)) & BMSR_ANEGCOMPLETE) {}

	/* Determine operating mode */
	if (stat & BMSR_ESTATEN) {
		tmp1 = greth_mdio_read(dev, MII_CTRL1000);
		tmp2 = greth_mdio_read(dev, MII_STAT1000);

		if ((tmp1 & ADVERTISE_1000FULL) && (tmp2 & LPA_1000FULL)) {
			net_speed = NET_1000FULL;
		}
		if ((tmp1 & ADVERTISE_1000HALF) && (tmp2 & LPA_1000HALF)) {
			net_speed = NET_1000HALF;
		}

		if (net_is_1000(net_speed) && !gbit_mac) {
			net_speed = NET_10HALF;
			greth_mdio_write(dev, MII_BMCR, BMCR_SPEED100);
		}
	}
	else {
		tmp1 = greth_mdio_read(dev, MII_ADVERTISE);
		tmp2 = greth_mdio_read(dev, MII_LPA);

		if ((tmp1 & ADVERTISE_100FULL) && (tmp2 & LPA_100FULL)) {
			net_speed = NET_100FULL;
		}
		if ((tmp1 & ADVERTISE_100HALF) && (tmp2 & LPA_100HALF)) {
			net_speed = NET_100HALF;
		}
		if ((tmp1 & ADVERTISE_10FULL) && (tmp2 & LPA_10FULL)) {
			net_speed = NET_10FULL;
		}
	}

auto_neg_done:
	/* Set speed and duplex bits in control register */
	greth_set_speed(dev, net_speed);
}

static void greth_reset(struct net_device *dev) {
	struct greth_regs *regs;

	log_debug("Resetting GRETH");

	regs = GRETH_REGS(dev);

	/* Reset core */
	REG32_STORE(&regs->control, GRETH_CTRL_RS);

	/* Wait for core to finish reset cycle */
	greth_wait_bit_reset(&regs->control, GRETH_CTRL_RS);

	/* Init PHY */
	greth_init_phy(dev);
}

static int greth_dev_init(struct net_device *dev) {
	struct greth_priv *greth;
	int err;

	err = 0;
	greth = netdev_priv(dev);

	/* Init base address and irq number */
	__greth_dev_init(dev);

	dev->drv_ops = &greth_ops;

	/* Allocate descriptors */
	greth->txbd_base = (struct greth_bd *)memalign(0x400,
	    GRETH_TXBD_CNT * sizeof(struct greth_bd));
	greth->rxbd_base = (struct greth_bd *)memalign(0x400,
	    GRETH_RXBD_CNT * sizeof(struct greth_bd));

	/* Allocate buffer pointers */
	greth->rxbuf_base = (struct sk_buff **)malloc(
	    sizeof(struct sk_buff *) * GRETH_RXBD_CNT);

	if (!greth->txbd_base || !greth->rxbd_base || !greth->rxbuf_base) {
		err = -errno;
		log_error("Unable to allocate memory");
		goto out;
	}

out:
	return err;
}

static void greth_real_init(struct net_device *dev) {
	struct greth_priv *greth;
	struct greth_regs *regs;
	struct greth_bd bd;
	int i;

	regs = GRETH_REGS(dev);
	greth = netdev_priv(dev);

	/* Init tx decriptors */
	for (i = 0; i < GRETH_TXBD_CNT; i++) {
		bd.stat = 0;
		bd.addr = 0;

		if (i == GRETH_TXBD_CNT - 1) {
			bd.stat |= GRETH_BD_WR;
		}

		greth_bd_save(greth, greth->txbd_base + i, &bd);
	}

	/* Init rx decriptors */
	for (i = 0; i < GRETH_RXBD_CNT; i++) {
		/* Allocate socket buffers */
		greth->rxbuf_base[i] = skb_alloc(ETH_FRAME_LEN);

		bd.stat = GRETH_BD_EN | GRETH_BD_IE;
		bd.addr = (uint32_t)skb_data_cast_in(greth->rxbuf_base[i]->data);

		if (i == GRETH_RXBD_CNT - 1) {
			bd.stat |= GRETH_BD_WR;
		}

		greth_bd_save(greth, greth->rxbd_base + i, &bd);
	}

	/* Set current receive descriptor number */
	greth->rxbd_curr = 0;

	/* Set pointers to tx/rx descriptor areas */
	REG32_STORE(&regs->tx_desc_p, (uint32_t)&greth->txbd_base[0]);
	REG32_STORE(&regs->rx_desc_p, (uint32_t)&greth->rxbd_base[0]);

	/* Enable receiver */
	log_debug("Enable receiver");
	REG32_ORIN(&regs->control, GRETH_CTRL_RE | GRETH_CTRL_RI);
}

static int greth_init(void) {
	struct net_device *dev;
	int err;

	/* Allocate ethernet device */
	dev = etherdev_alloc(sizeof(struct greth_priv));
	if (dev == NULL) {
		err = -ENOMEM;
		goto out;
	}

	/* Init ethernet device */
	err = greth_dev_init(dev);
	if (err) {
		goto out;
	}

	greth_reset(dev);
	greth_real_init(dev);

	err = irq_attach(dev->irq, greth_irq_handler, 0, dev, "greth");
	if (err) {
		goto out;
	}

	err = inetdev_register_dev(dev);

out:
	return err;
}
