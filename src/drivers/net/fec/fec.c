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

#include <util/log.h>

#include <embox/unit.h>

#include <framework/mod/options.h>


struct fec_priv {
	uint32_t base_addr;
	struct fec_buf_desc *rbd_base;
	int rbd_index;
	struct fec_buf_desc *tbd_base;
	int tbd_index;
};

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

extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);

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
	assert((uint32_t)_tx_desc_ring == REG32_LOAD(ENET_TDSR));

	sp = ipl_save();
	{
		cur_tx_desc = priv->tbd_index;
		log_debug("Transmitting packet %2d", cur_tx_desc);

		memcpy(&_tx_buf[cur_tx_desc][0], data, skb->len);
		dcache_flush(&_tx_buf[cur_tx_desc][0], skb->len);
		//dcache_flush(data, skb->len);

		desc = &_tx_desc_ring[cur_tx_desc];
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

		int timeout = 0xFF;
		while(--timeout) {
			if (!(REG32_LOAD(ENET_TDAR))) {
				break;
			}
		}

		if (timeout == 0) {
			log_error("TX timeout ENET_TDAR is not zero...");
		}

		timeout = 0xFFFFFF;
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
	}
out1:
	ipl_restore(sp);

out:
	skb_free(skb);

	return res;
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
	REG32_STORE(ENET_EIMR, 0x0 | ENET_EIR_RXB | ENET_EIR_RXF);
	/*
	 * Clear FEC-Lite interrupt event register(IEVENT)
	 */
	REG32_STORE(ENET_EIR, 0xffc00000);

	/* Full-Duplex Enable */
	 REG32_STORE(ENET_TCR, (1 << 2));

	/* MAX_FL frame length*/
	/* Enables 10-Mbit/s mode of the RMII or RGMII ?*/
	/* MII or RMII mode, as indicated by the RMII_MODE field. */
#if (OPTION_GET(NUMBER, speed) == 1000)
	 REG32_STORE(ENET_RCR, 0x5ee0044);
#else
	 REG32_STORE(ENET_RCR, 0x5ee0104);
#endif
	/* Maximum Receive Buffer Size Register
	 * Receive buffer size in bytes. This value, concatenated with the four
	 * least-significant bits of this register (which are always zero),
	 * is the effective maximum receive buffer size.
	 */
	REG32_STORE(ENET_MRBR, 0x5f0);

	fec_rbd_init(dev->priv, RX_BUF_FRAMES, 0x600);

	fec_tbd_init(dev->priv);

	REG32_STORE(ENET_ECR, 0xF0000100);

	 /* Transmit FIFO Write 64 bytes */
	REG32_STORE(ENET_TFWR, 0x100);

	REG32_STORE(ENET_ECR, REG32_LOAD(ENET_ECR) | ENET_ETHEREN); /* Note: should be last ENET-related init step */

	REG32_STORE(ENET_RDAR, (1 << 24));

	fec_reg_dump("ENET dump embox...\n");
}

static int fec_open(struct net_device *dev) {

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

	while(1) {
		desc = &_rx_desc_ring[priv->rbd_index];
		dcache_inval(desc, sizeof(struct fec_buf_desc));
		if (desc->flags & FLAG_E) {
			break;
		}

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

		priv->rbd_index = (priv->rbd_index + 1) % RX_BUF_FRAMES;

		desc->flags |= FLAG_R;
		dcache_flush(desc, sizeof(struct fec_buf_desc));

		REG32_STORE(ENET_RDAR, (1 << 24));
	}

	return res;
}

static irq_return_t imx6_irq_handler(unsigned int irq_num, void *dev_id) {
	uint32_t state;
	struct fec_priv *priv;

	assert(dev_id);

	priv = ((struct net_device *)dev_id)->priv;

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

static const struct net_driver fec_drv_ops = {
	.xmit = fec_xmit,
	.start = fec_open,
	.set_macaddr = fec_set_macaddr
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

	tmp = irq_attach(ENET_IRQ, imx6_irq_handler, 0, nic, "i.MX6 enet");
	if (tmp)
		return tmp;

	return inetdev_register_dev(nic);
}

static struct periph_memory_desc fec_mem = {
	.start = NIC_BASE,
	.len   = 0x200,
};

PERIPH_MEMORY_DEFINE(fec_mem);
