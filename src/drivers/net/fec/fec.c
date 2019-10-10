/**
 * @file fec.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2016-08-11
 */

#include "fec.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <drivers/common/memory.h>

#include <hal/reg.h>
#include <kernel/irq.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>
#include <net/mii.h>

#include <util/log.h>

#include <embox/unit.h>

#include <framework/mod/options.h>
/* FEC MII MMFR bits definition */
#define FEC_MMFR_ST     (1 << 30)
#define FEC_MMFR_OP_READ    (2 << 28)
#define FEC_MMFR_OP_WRITE   (1 << 28)
#define FEC_MMFR_PA(v)      ((v & 0x1f) << 23)
#define FEC_MMFR_RA(v)      ((v & 0x1f) << 18)
#define FEC_MMFR_TA     (2 << 16)
#define FEC_MMFR_DATA(v)    (v & 0xffff)

static struct fec_priv fec_priv;

static void fec_reg_dump(const char * title) {
	log_debug("%s", title);

	log_debug("ENET_EIR  %10x %10x", ENET_EIR, REG32_LOAD(ENET_EIR ));
	log_debug("ENET_EIMR %10x %10x", ENET_EIMR, REG32_LOAD(ENET_EIMR));
	log_debug("ENET_RDAR %10x %10x", ENET_RDAR, REG32_LOAD(ENET_RDAR));
	log_debug("ENET_TDAR %10x %10x", ENET_TDAR, REG32_LOAD(ENET_TDAR));
	log_debug("ENET_ECR  %10x %10x", ENET_ECR, REG32_LOAD(ENET_ECR ));
	log_debug("ENET_MMFR %10x %10x", ENET_MMFR, REG32_LOAD(ENET_MMFR));
	log_debug("ENET_MSCR %10x %10x", ENET_MSCR, REG32_LOAD(ENET_MSCR));
	log_debug("ENET_MIBC %10x %10x", ENET_MIBC, REG32_LOAD(ENET_MIBC));
	log_debug("ENET_RCR  %10x %10x", ENET_RCR, REG32_LOAD(ENET_RCR ));
	log_debug("ENET_TCR  %10x %10x", ENET_TCR, REG32_LOAD(ENET_TCR ));
	log_debug("ENET_MAC_LOW   %10x %10x", ENET_MAC_LOW, REG32_LOAD(ENET_MAC_LOW  ));
	log_debug("ENET_MAC_HI    %10x %10x", ENET_MAC_HI, REG32_LOAD(ENET_MAC_HI   ));
	log_debug("ENET_OPD  %10x %10x", ENET_OPD, REG32_LOAD(ENET_OPD));
	log_debug("ENET_IAUR %10x %10x", ENET_IAUR, REG32_LOAD(ENET_IAUR));
	log_debug("ENET_IALR %10x %10x", ENET_IALR, REG32_LOAD(ENET_IALR));
	log_debug("ENET_GAUR %10x %10x", ENET_GAUR, REG32_LOAD(ENET_GAUR));
	log_debug("ENET_GALR %10x %10x", ENET_GALR, REG32_LOAD(ENET_GALR));
	log_debug("ENET_TFWR %10x %10x", ENET_TFWR, REG32_LOAD(ENET_TFWR));
	log_debug("ENET_WTF1 %10x %10x", ENET_WTF1, REG32_LOAD(ENET_WTF1));
	log_debug("ENET_WTF2 %10x %10x", ENET_WTF2, REG32_LOAD(ENET_WTF2));
	log_debug("ENET_RDSR %10x %10x", ENET_RDSR, REG32_LOAD(ENET_RDSR));
	log_debug("ENET_TDSR %10x %10x", ENET_TDSR, REG32_LOAD(ENET_TDSR));
	log_debug("ENET_MRBR %10x %10x", ENET_MRBR, REG32_LOAD(ENET_MRBR));
}

extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);

static void emac_set_macaddr(unsigned char _macaddr[6]) {
	uint32_t mac_hi, mac_lo;
	const uint8_t *tmp = _macaddr;

	REG32_STORE(ENET_IAUR, 0);
	REG32_STORE(ENET_IALR, 0);
	REG32_STORE(ENET_GAUR, 0);
	REG32_STORE(ENET_GALR, 0);

	log_debug("addr = %x:%x:%x:%x:%x:%x", tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);

	mac_hi  = (_macaddr[5] << 16) |
	          (_macaddr[4] << 24);
	mac_lo  = (_macaddr[3] <<  0) |
	          (_macaddr[2] <<  8) |
	          (_macaddr[1] << 16) |
	          (_macaddr[0] << 24);

	REG32_STORE(ENET_MAC_LOW, mac_lo);
	REG32_STORE(ENET_MAC_HI, mac_hi | 0x8808);
}

static struct fec_buf_desc _tx_desc_ring[TX_BUF_FRAMES] __attribute__ ((aligned(0x10)));
static struct fec_buf_desc _rx_desc_ring[RX_BUF_FRAMES] __attribute__ ((aligned(0x10)));

static uint8_t _tx_buf[TX_BUF_FRAMES][2048] __attribute__ ((aligned(0x10)));
static uint8_t _rx_buf[RX_BUF_FRAMES][2048] __attribute__ ((aligned(0x10)));

static void fec_tbd_init(struct fec_priv *fec)
{
	unsigned size = TX_BUF_FRAMES * sizeof(struct fec_buf_desc);

	memset(fec->tbd_base, 0, size);

	fec->tbd_base[TX_BUF_FRAMES - 1].flags = FLAG_W;

	dcache_flush(fec->tbd_base, size);

	fec->tbd_index = 0;
	REG32_STORE(ENET_TDSR, ((uint32_t) fec->tbd_base));
}

static void fec_rbd_init(struct fec_priv *fec, int count, int dsize) {
	uint32_t size;
	int i;

	/*
	 * Reload the RX descriptors with default values and wipe
	 * the RX buffers.
	 */
	size = count * sizeof(struct fec_buf_desc);
	for (i = 0; i < count; i++) {
		fec->rbd_base[i].data_pointer = (uint32_t)&_rx_buf[i][0];
		fec->rbd_base[i].len = 0;
		fec->rbd_base[i].flags = FLAG_R;
	}

	/* Mark the last RBD to close the ring. */
	fec->rbd_base[count - 1].flags = FLAG_W | FLAG_R;

	fec->rbd_index = 0;

	dcache_flush((void *)fec->rbd_base, size);

	REG32_STORE(ENET_RDSR, (uint32_t)fec->rbd_base);
}

static int fec_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint8_t *data;
	struct fec_buf_desc *desc;
	int res;
	ipl_t sp;
	struct fec_priv *priv;
	int cur_tx_desc;

	assert(dev);
	assert(skb);

	res = 0;

	data = (uint8_t*) skb_data_cast_in(skb->data);

	if (!data) {
		log_error("No skb data!\n");
		res = -1;
		goto out;

	}
	show_packet(data, skb->len, "tx");

	priv = dev->priv;
	dcache_inval(priv, sizeof(struct fec_priv));
	assert((uint32_t)priv->tbd_base == REG32_LOAD(ENET_TDSR));

	sp = ipl_save();
	{
		cur_tx_desc = priv->tbd_index;
		log_debug("Transmitting packet %2d", cur_tx_desc);

		memcpy(&_tx_buf[cur_tx_desc][0], data, skb->len);
		dcache_flush(&_tx_buf[cur_tx_desc][0], skb->len);
		//dcache_flush(data, skb->len);

		desc = &priv->tbd_base[cur_tx_desc];
		dcache_inval(desc, sizeof(struct fec_buf_desc));
		if (desc->flags & FLAG_R) {
			log_error("tx desc still busy");
			goto out1;
		}

		desc->data_pointer = (uint32_t)&_tx_buf[cur_tx_desc][0];
		//desc->data_pointer = (uint32_t)data;
		desc->len          = skb->len;
		desc->flags       |= FLAG_L | FLAG_TC | FLAG_R;
		dcache_flush(desc, sizeof(struct fec_buf_desc));

		REG32_LOAD(desc + sizeof(*desc) - 4);

		REG32_STORE(ENET_TDAR, 1 << 24);
		__asm__("nop");

		int timeout = 0xFFF;
		while(--timeout) {
			dcache_inval((void*) ENET_TDAR, 4);
			if (!(REG32_LOAD(ENET_TDAR))) {
				break;
			}
		}

		if (timeout == 0) {
			log_error("TX timeout ENET_TDAR is not zero...");
		}

		timeout = 0xFFFF;
		while(--timeout) {
			dcache_inval(desc, sizeof(struct fec_buf_desc));
			if (!(desc->flags & FLAG_R)) {
				break;
			}
		}
		if (timeout == 0) {
			log_error("TX timeout bit READY still set...");
		}

		priv->tbd_index = (priv->tbd_index + 1) % TX_BUF_FRAMES;
		dcache_flush(priv, sizeof(struct fec_priv));
	}
out1:
	ipl_restore(sp);

out:
	skb_free(skb);
	return res;
}

static void fec_mdio_init(struct net_device *dev) {
	phy_detect(dev);
	phy_autoneg(dev, FEC_SPEED);
}

static void _reset(struct net_device *dev) {
	int cnt = 0;

	fec_reg_dump("ENET dump uboot...");

	REG32_STORE(ENET_ECR, ENET_RESET);
	while(REG32_LOAD(ENET_ECR) & ENET_RESET){
		if (cnt ++ > 100000) {
			log_error("enet can't be reset");
			break;
		}
	}

	/* set mii speed */
	REG32_STORE(ENET_MSCR, 0x1a);


	/*
	 * Set interrupt mask register
	 */
	/* enable all interrupts */
	/* Transmit Frame Interrupt
	 * Transmit Buffer Interrupt
	 * Receive Frame Interrupt
	 * Receive Buffer Interrupt
	 */
	REG32_STORE(ENET_EIMR, ENET_EIR_RXB | ENET_EIR_RXF);
	/* Clear FEC-Lite interrupt event register(IEVENT) */
	REG32_STORE(ENET_EIR, 0xffc00000);

	/* Full-Duplex Enable */
	REG32_STORE(ENET_TCR, (1 << 2));

	/* MAX_FL frame length*/

	fec_mdio_init(dev);

	/* Enables 10-Mbit/s mode of the RMII or RGMII ?*/
	/* Maximum Receive Buffer Size Register
	 * Receive buffer size in bytes. This value, concatenated with the four
	 * least-significant bits of this register (which are always zero),
	 * is the effective maximum receive buffer size. */
	REG32_STORE(ENET_MRBR, 0x5f0);

	fec_rbd_init(dev->priv, RX_BUF_FRAMES, 0x600);

	fec_tbd_init(dev->priv);

	/* Transmit FIFO Write 64 bytes */
	REG32_STORE(ENET_TFWR, 0x100);

	/* Note: this should be last ENET-related init step */
	REG32_STORE(ENET_ECR, REG32_LOAD(ENET_ECR) | ENET_ETHEREN);

	REG32_STORE(ENET_RDAR, (1 << 24));
	fec_reg_dump("ENET dump embox...\n");
}

static int fec_open(struct net_device *dev) {
	assert(dev);

	_reset(dev);

	return 0;
}

static int fec_set_macaddr(struct net_device *dev, const void *addr) {
	assert(dev);
	assert(addr);

	emac_set_macaddr((unsigned char *)addr);

	return 0;
}

static int imx6_receive(struct net_device *dev_id, struct fec_priv *priv) {
	struct fec_buf_desc *desc;
	struct sk_buff *skb;
	int res = 0;

	desc = &_rx_desc_ring[priv->rbd_index];
	dcache_inval(desc, sizeof(struct fec_buf_desc));
	if (desc->flags & FLAG_E) {
		/* IRQ happened, but buf is empty, so check
		 * other buffer frames */
		for (int i = 0; i < RX_BUF_FRAMES; i++) {
			desc = &_rx_desc_ring[i];
			dcache_inval(desc, sizeof(struct fec_buf_desc));
			if (!(desc->flags & FLAG_E)) {
				log_debug("found frame %d, should be %d", i, priv->rbd_index);
				priv->rbd_index = i;
				break;
			}
		}
	}


	while(!(desc->flags & FLAG_E)) {
		log_debug("Receiving packet %d", priv->rbd_index);

		priv->rbd_index = (priv->rbd_index + 1) % RX_BUF_FRAMES;
		dcache_flush(priv, sizeof(struct fec_priv));

		skb = skb_alloc(desc->len);
		if (!skb) {
			log_error("can't allocate skb");
			break;
		}
		dcache_inval((void *)desc->data_pointer, desc->len);
		memcpy(skb_data_cast_in(skb->data),
				(void*)desc->data_pointer, desc->len);
		skb->len = desc->len; /* without CRC */
		skb->dev = dev_id;

		show_packet(skb_data_cast_in(skb->data), skb->len, "rx");

		netif_rx(skb);

		desc->flags |= FLAG_R;
		dcache_flush(desc, sizeof(struct fec_buf_desc));

		REG32_STORE(ENET_RDAR, (1 << 24));

		desc = &_rx_desc_ring[priv->rbd_index];
		dcache_inval(desc, sizeof(struct fec_buf_desc));
	}

	return res;
}

static irq_return_t imx6_irq_handler(unsigned int irq_num, void *dev_id) {
	uint32_t state;
	struct fec_priv *priv;

	assert(dev_id);

	priv = ((struct net_device *)dev_id)->priv;
	dcache_inval(priv, sizeof(struct fec_priv));
	state = REG32_LOAD(ENET_EIR);

	log_debug("Interrupt mask %#010x", state);

	if (state & ENET_EIR_EBERR) {
		log_error("Ethernet bus error, resetting ENET!");
		//REG32_STORE(ENET_ECR, RESET);
		_reset(dev_id);

		return IRQ_HANDLED;
	}

	if (state & (ENET_EIR_RXB | ENET_EIR_RXF)) {
		imx6_receive(dev_id, priv);
	}

	REG32_STORE(ENET_EIR, state);

	return IRQ_HANDLED;
}

/* MII-related definitios */
#define FEC_IEVENT_MII          0x00800000
#define FEC_MII_DATA_ST		0x40000000	/* Start of frame delimiter */
#define FEC_MII_DATA_OP_RD	0x20000000	/* Perform a read operation */
#define FEC_MII_DATA_OP_WR	0x10000000	/* Perform a write operation */
#define FEC_MII_DATA_PA_MSK	0x0f800000	/* PHY Address field mask */
#define FEC_MII_DATA_RA_MSK	0x007c0000	/* PHY Register field mask */
#define FEC_MII_DATA_TA		0x00020000	/* Turnaround */
#define FEC_MII_DATA_DATAMSK	0x0000ffff	/* PHY data field */

#define FEC_MII_DATA_RA_SHIFT	18	/* MII Register address bits */
#define FEC_MII_DATA_PA_SHIFT	23	/* MII PHY address bits */

#define FEC_MII_DATA_RA_SHIFT	18	/* MII Register address bits */
#define FEC_MII_DATA_PA_SHIFT	23	/* MII PHY address bits */
static int fec_mdio_read(struct net_device *dev, uint8_t regAddr) {
	struct fec_priv *fec = dev->priv;
	uint32_t reg;		/* convenient holder for the PHY register */
	uint32_t phy;		/* convenient holder for the PHY */
	int val;
	int retry = 0;

	/* Reading from any PHY's register is done by properly
	 * programming the FEC's MII data register. */
	REG32_STORE(ENET_EIR, FEC_IEVENT_MII);
	reg = regAddr << FEC_MII_DATA_RA_SHIFT;
	phy = fec->phy_id << FEC_MII_DATA_PA_SHIFT;

	REG32_STORE(ENET_MMFR, FEC_MII_DATA_ST | FEC_MII_DATA_OP_RD |
			FEC_MII_DATA_TA | phy | reg);

	/* Wait for the related interrupt */
	while (!(REG32_LOAD(ENET_EIR) & FEC_IEVENT_MII)) {
		if (retry++ > 0xffff) {
			log_error("MDIO write failed");
			return -1;
		}
	}

	/* Clear MII interrupt bit */
	REG32_STORE(ENET_EIR, FEC_IEVENT_MII);

	/* It's now safe to read the PHY's register */
	val = REG32_LOAD(ENET_MMFR) & 0xFFFF;
	log_debug("reg:%02x val:%#x", regAddr, val);
	return val;
}

static int fec_mdio_write(struct net_device *dev, uint8_t regAddr, uint16_t data) {
	struct fec_priv *fec = dev->priv;
	uint32_t reg;		/* convenient holder for the PHY register */
	uint32_t phy;		/* convenient holder for the PHY */
	int retry = 0;

	reg = regAddr << FEC_MII_DATA_RA_SHIFT;
	phy = fec->phy_id << FEC_MII_DATA_PA_SHIFT;

	REG32_STORE(ENET_MMFR, FEC_MII_DATA_ST | FEC_MII_DATA_OP_WR |
		FEC_MII_DATA_TA | phy | reg | data);

	/* Wait for the MII interrupt */
	while (!(REG32_LOAD(ENET_EIR) & FEC_IEVENT_MII)) {
		if (retry++ > 0xffff) {
			log_error("MDIO write failed");
			return -1;
		}
	}

	/* Clear MII interrupt bit */
	REG32_STORE(ENET_EIR, FEC_IEVENT_MII);
	log_debug("reg:%02x val:%#x", regAddr, data);

	return 0;
}

static void fec_set_phyid(struct net_device *dev, uint8_t phyid) {
	struct fec_priv *fec = dev->priv;
	fec->phy_id = phyid;
}

static int fec_set_speed(struct net_device *dev, int speed) {
	speed = net_to_mbps(speed);

	if (speed != FEC_SPEED && FEC_SPEED != 0) {
		log_error("Can't set %dmbps as driver is configured "
				"to force %dmbps", speed, FEC_SPEED);
		return -1;
	}

	switch (speed) {
	case 1000:
		REG32_STORE(ENET_RCR, 0x5ee0000 | ENET_RCR_RGMII_EN);
		REG32_STORE(ENET_ECR, 0xF0000100 | ENET_SPEED);
		break;
	case 100:
		REG32_STORE(ENET_RCR, 0x5ee0000 | ENET_RCR_RGMII_EN);
		REG32_STORE(ENET_ECR, 0xF0000100);
		break;
	case 10:
		REG32_STORE(ENET_RCR, 0x5ee0000 | ENET_RCR_RMII_10T |
				ENET_RCR_RGMII_EN);
		REG32_STORE(ENET_ECR, 0xF0000100);
		break;
	}

	return 0;
}

static const struct net_driver fec_drv_ops = {
	.xmit        = fec_xmit,
	.start       = fec_open,
	.set_macaddr = fec_set_macaddr,
	.mdio_read   = fec_mdio_read,
	.mdio_write  = fec_mdio_write,
	.set_phyid   = fec_set_phyid,
	.set_speed   = fec_set_speed
};

EMBOX_UNIT_INIT(fec_init);
static int fec_init(void) {
	struct net_device *nic;
	int tmp;

	if (NULL == (nic = etherdev_alloc(0))) {
		return -ENOMEM;
	}

	nic->drv_ops = &fec_drv_ops;

	fec_priv.base_addr = NIC_BASE;
	fec_priv.rbd_base =  _rx_desc_ring;
	fec_priv.tbd_base =  _tx_desc_ring;
	nic->priv = &fec_priv;

	tmp = irq_attach(ENET_IRQ, imx6_irq_handler, 0, nic, "FEC");
	if (tmp)
		return tmp;

	return inetdev_register_dev(nic);
}

PERIPH_MEMORY_DEFINE(fec, NIC_BASE, 0x200);
