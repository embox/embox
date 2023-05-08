//--------------------------------------------------------------
// @author  V. Syrtsov
//--------------------------------------------------------------

#include <util/log.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <kernel/irq.h>

#include <hal/reg.h>

#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>

#include <util/binalign.h>

#include <drivers/common/memory.h>

#include <driver/phy_rcm_sgmii.h>
#include <driver/rcm_mdio.h>

#include <embox/unit.h>

#include "rcm_mgeth.h"

#define BIT(b) ( 1 << (b))

#define MGPIO0_BASE    (0x01084000)

EMBOX_UNIT_INIT(mgeth_init);

#define BASE_ADDR   OPTION_GET(NUMBER, base_addr)
#define MGETH_IRQ   OPTION_GET(NUMBER, irq_num)


extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);


#define ETH_PHY_ID                0xBBCD

#define AUTONEGOTIATION_EN          0

#define MGETH_RXBD_CNT 8 /* CNT + link */
#define MGETH_TXBD_CNT 4 /* CNT + link */

#define MGETH_RXBUF_SIZE      1536 /* 0x600 */


struct mgeth_priv {
	struct mgeth_regs *regs;

	struct rcm_mgeth_long_desc rxbd_base[MGETH_RXBD_CNT + 1] __attribute__ ((aligned (16)));
	int rxbd_no;
	uint8_t rx_buffs[MGETH_RXBD_CNT][MGETH_RXBUF_SIZE] __attribute__ ((aligned (8)));


	struct rcm_mgeth_long_desc txbd_base[MGETH_TXBD_CNT + 1] __attribute__ ((aligned (16)));
	int txbd_no;
	volatile struct sk_buff *tx_active_skb[MGETH_TXBD_CNT];

};


static int mgeth_xmit(struct net_device *dev, struct sk_buff *skb);
static int mgeth_open(struct net_device *dev);
static int mgeth_close(struct net_device *dev);
static int mgeth_set_mac(struct net_device *dev, const void *addr);


static int mgeth_rxd_init(struct mgeth_priv *priv, int idx) {
	priv->rxbd_base[idx].usrdata_h = 0;
	priv->rxbd_base[idx].usrdata_l = 0;

	if (idx == (MGETH_RXBD_CNT)) {
		/* link descriptor */
		priv->rxbd_base[idx].flags_length = MGETH_BD_LINK;
		priv->rxbd_base[idx].memptr = (uint32_t) (uintptr_t) (&priv->rxbd_base[0]);
	} else {
		 /* usual descriptor */
		priv->rxbd_base[idx].flags_length = MGETH_BD_INT | MGETH_RXBUF_SIZE;
		priv->rxbd_base[idx].memptr = (uint32_t) (uintptr_t) (&priv->rx_buffs[idx][0]);

	}
	dcache_flush(&priv->rxbd_base[idx], sizeof(struct rcm_mgeth_long_desc));

	return 0;
}

static inline int mgeth_txd_init(struct mgeth_priv *priv, int idx, uint32_t memptr, int len) {
	priv->txbd_base[idx].usrdata_h = 0;
	priv->txbd_base[idx].usrdata_l = 0;

	if (idx == (MGETH_RXBD_CNT)) {
		/* link descriptor */
		priv->txbd_base[idx].flags_length = MGETH_BD_STOP | MGETH_BD_LINK;
		priv->txbd_base[idx].memptr = (uint32_t) (uintptr_t) (&priv->rxbd_base[0]);
	} else {
		 /* usual descriptor */
		priv->txbd_base[idx].flags_length = MGETH_BD_STOP | MGETH_BD_INT | len;
		priv->txbd_base[idx].memptr = memptr;

	}
	dcache_flush(&priv->txbd_base[idx], sizeof(struct rcm_mgeth_long_desc));
	return 0;
}

static void mgeth_set_packet_filter(const struct mgeth_regs *regs, unsigned char dev_addr[6]) {

	REG32_STORE(&regs->rx[0].rx_eth_mask_value[0],
			dev_addr[0] << 0 |
			dev_addr[1] << 8 |
			dev_addr[2] << 16 |
			dev_addr[3] << 24);

	REG32_STORE(&regs->rx[0].rx_eth_mask_value[1],
			dev_addr[4] << 0 |
			dev_addr[5] << 8);

	REG32_STORE(&regs->rx[0].rx_eth_mask_activ[0], 0xffffffff);
	REG32_STORE(&regs->rx[0].rx_eth_mask_activ[1], 0x0000ffff);

	REG32_STORE(&regs->mg_len_mask_ch0, 0);
}


static inline void mgeth_reset_mac(const struct mgeth_regs *regs) {
	REG32_STORE(&regs->sw_rst, 0x1);

	while (REG32_LOAD(&regs->sw_rst) & 0x1) {
		;
	}
}

static inline void mgeth_init_mac(const struct mgeth_regs *regs, int speed, int duplex) {
	uint32_t ctrl;

	ctrl = REG32_LOAD(&regs->mg_control);
	ctrl &= ~0x7;

	if (duplex) {
		ctrl |= RCM_MGETH_CTRL_FD_M;
	}

	ctrl |= speed << RCM_MGETH_CTRL_SPEED_OFF;

	REG32_STORE(&regs->mg_control, ctrl);
}

/* init/start hardware and allocate descriptor buffers for rx side
 *
 */
static int mgeth_start(struct net_device *dev) {
	int i;
	struct mgeth_priv *priv = dev->priv;

	mgeth_reset_mac(priv->regs);

	mgeth_init_mac(priv->regs, MGETH_SPEED_1000, 1);

	/* disable interrupts */
	REG32_STORE(&priv->regs->mg_irq_mask, 0);

	/* set filters for given MAC */
	mgeth_set_packet_filter(priv->regs, dev->dev_addr);

	/* setup RX queues */
	{
		/* Initiate rx descriptors */
		for (i = 0; i <= (MGETH_RXBD_CNT); i++) {
			mgeth_rxd_init(priv, i);
		}
		/* initiate indexes */
		priv->rxbd_no = 0;

		/* Set pointer to rx descriptor areas */
		REG32_STORE(&priv->regs->rx[0].dma_regs.settings,
				MGETH_CHAN_DESC_LONG | MGETH_CHAN_ADD_INFO |
				(sizeof(struct rcm_mgeth_long_desc) << MGETH_CHAN_DESC_GAP_SHIFT));
		REG32_STORE(&priv->regs->rx[0].dma_regs.desc_addr, (uint32_t )(uintptr_t)(&priv->rxbd_base[0]));

		REG32_STORE(&priv->regs->rx[0].dma_regs.irq_mask, (1 << 2));

		/* enable rx */
		REG32_STORE(&priv->regs->rx[0].dma_regs.enable, MGETH_ENABLE);
	}


	/* setup RX queues */
	{
		/* Initiate tx descriptors */
		for (i = 0; i < (MGETH_TXBD_CNT ); i++) {
			mgeth_txd_init(priv, i, 0, 0);
		}
		memset(priv->tx_active_skb, 0, sizeof(priv->tx_active_skb));

		priv->txbd_no = 0;

		REG32_STORE(&priv->regs->tx[0].dma_regs.settings,
			MGETH_CHAN_DESC_LONG | MGETH_CHAN_ADD_INFO |
				(sizeof(struct rcm_mgeth_long_desc) << MGETH_CHAN_DESC_GAP_SHIFT));
		REG32_STORE(&priv->regs->tx[0].dma_regs.irq_mask, (1 << 2));
	}


	return 0;
}

/* Stop the hardware from looking for packets - may be called even if
 *	 state == PASSIVE
 */
static void mgeth_stop(struct net_device *dev) {
	struct mgeth_priv *priv = dev->priv;

	REG32_STORE(&priv->regs->rx[0].dma_regs.enable, 0);
	REG32_STORE(&priv->regs->tx[0].dma_regs.enable, 0);
}

/* Send the bytes passed in "packet" as a packet on the wire */
static int mgeth_send(struct mgeth_priv *priv, struct sk_buff *skb) {
	struct mgeth_tx_regs *mgeth_tx_regs = &priv->regs->tx[0];
	int len = skb->len;

	/* copy to internal buffer if size less than min */
	if (len < ETH_ZLEN) {
		len = ETH_ZLEN;
	}

//	irq_lock();
//	if (priv->txbd_no != 0) {
//
//	}
//	irq_unlock();
	dcache_flush(skb->mac.raw, len);

	/* wait for completion of previous transfer */
//	while ( (REG32_LOAD(&mgeth_tx_regs->dma_regs.enable) & MGETH_ENABLE) != 0) {
//		;
//	}

	while(priv->tx_active_skb[0] != NULL) ;

	priv->tx_active_skb[0] = skb;


	mgeth_txd_init(priv, 0, (uint32_t)(uintptr_t) skb->mac.raw, len);

	/* enable tx */
	REG32_STORE(&mgeth_tx_regs->dma_regs.desc_addr,
			(uint32_t)(uintptr_t)&priv->txbd_base[0]);
	REG32_STORE(&mgeth_tx_regs->dma_regs.enable, MGETH_ENABLE);

	return 0;
}

static int mgeth_rx_chan(struct net_device *nic_p, int chan,
		struct sk_buff *rx_skb[MGETH_RXBD_CNT]) {
	uint32_t dma_status;
	int cnt = 0;
	struct mgeth_priv *priv = nic_p->priv;

	dma_status = REG32_LOAD(&priv->regs->rx[chan].dma_regs.status);
	log_debug("rx_chan (%i) dma status (0x%x)", chan, dma_status);

	if (!(dma_status & (1 << 2))) {
		return 0;
	}

	irq_lock();
	{
		struct rcm_mgeth_long_desc *curr_bd;

		while (cnt < MGETH_RXBD_CNT) {
			int len;
			struct sk_buff *skb;
			unsigned char *buf;

			curr_bd = &priv->rxbd_base[priv->rxbd_no];
			dcache_inval(curr_bd, sizeof(struct rcm_mgeth_long_desc));

			if (!(curr_bd->flags_length & MGETH_BD_OWN)) {
				//log_error("break rx_bd idx %d", priv->rxbd_no);
				break;
			}

			if (curr_bd->flags_length & MGETH_BD_LINK) {
				//log_error("continue MGETH_BD_LINK %d", priv->rxbd_no);
				mgeth_rxd_init(priv, MGETH_RXBD_CNT);
				priv->rxbd_no = 0;

				continue;
			}

			assert(priv->rxbd_no < MGETH_RXBD_CNT);

			len = curr_bd->flags_length & MGETH_BD_LEN_MASK;

			if (len != 0) {

				buf = (void *)((uintptr_t)(curr_bd->memptr));
				dcache_inval(buf, len);

				skb = skb_alloc(len);

				if (skb == NULL) {
					log_error("skb_alloc failed drop an ethernet frame");
					show_packet(buf, len, "dropped rx");
					continue;
				} else {

					memcpy(skb->mac.raw, buf, len);
					skb->dev = nic_p;

					rx_skb[cnt] = skb;
					cnt++;
				}
			} else {
				log_debug("rx desc len =0 %d", priv->rxbd_no);
			}

			mgeth_rxd_init(priv, priv->rxbd_no);
			priv->rxbd_no++;

		}

	}
	irq_unlock();

	return cnt;
}

static void mgeth_xmit_complite(struct mgeth_priv *mgeth_priv, int i) {
	uint32_t dma_status;
	struct sk_buff *skb;

	dma_status = REG32_LOAD(&mgeth_priv->regs->tx[i].dma_regs.status);
	log_debug("tx_chan (%i) dma status (0x%x)", i, dma_status);

	irq_lock();
//	dcache_inval(&mgeth_priv->txbd_base[0], sizeof(struct rcm_mgeth_long_desc));
	skb = (struct sk_buff *)mgeth_priv->tx_active_skb[0];

	mgeth_priv->tx_active_skb[0] = NULL;

	irq_unlock();

	skb_free(skb);
}

static irq_return_t mgeth_irq_handler(unsigned int irq_num, void *dev_id) {
	struct net_device *nic_p = dev_id;
	uint32_t global_status;
	struct mgeth_priv *mgeth_priv;
	int i;


	if (!nic_p) {
		return IRQ_NONE;
	}

	mgeth_priv = nic_p->priv;

	global_status = REG32_LOAD(&mgeth_priv->regs->global_status);
	log_debug("global status (0x%x)", global_status);

//	irq_lock();
	{
		for (i = 0; i < RCM_MGETH_MAX_DMA_CHANNELS; i++) {
			if (global_status & BIT(i)) {
				mgeth_xmit_complite(mgeth_priv, i);
			}
			if (global_status & BIT(i + 16)) {
				int rx_cnt;
				int j;
				struct sk_buff *rx_skb[MGETH_RXBD_CNT];
				rx_cnt = mgeth_rx_chan(nic_p, i, rx_skb);
				//log_error("got %d, ind=%d)", rx_cnt, mgeth_priv->rxbd_no);
				for (j = 0; j < rx_cnt; j ++) {
					struct sk_buff *skb = rx_skb[j];

					show_packet(skb->mac.raw, skb->len, "rx");
					netif_rx(skb);
				}
			}
		}
	}
//	irq_unlock();

	return IRQ_HANDLED;
}

static int mgeth_xmit(struct net_device *dev, struct sk_buff *skb) {
	if (dev == NULL) {
		return -EINVAL;
	}

	mgeth_send(dev->priv, skb);
	show_packet(skb->mac.raw, skb->len, "tx");

	return ENOERR;
}


static int mgeth_open(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}
	mgeth_start(dev);

	return ENOERR;
}

static int mgeth_close(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}	
	mgeth_stop(dev);

	return ENOERR;
}

static int mgeth_set_mac(struct net_device *dev, const void *addr) {
	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}
	mgeth_stop(dev);
	memcpy(dev->dev_addr, addr, ETH_ALEN);

	mgeth_start(dev);

	return ENOERR;
}

extern void rcm_mgpio_init(uint32_t mgpio_base);

static const struct net_driver rcm_mgeth_ops = {
		.xmit = mgeth_xmit,
		.start = mgeth_open,
		.stop = mgeth_close,
		.set_macaddr = mgeth_set_mac,
};

static struct mgeth_priv mgeth_priv;
/*
 * initializing procedure
 */
static struct net_device *mgeth_netdev;
static int mgeth_init(void) {
	int res;
	struct net_device *nic;
	unsigned char hw_addr[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x02 };
	int i;

	nic = (struct net_device*) etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &rcm_mgeth_ops;
	nic->irq = MGETH_IRQ;
	nic->base_addr = BASE_ADDR;
	nic->priv = &mgeth_priv;

	mgeth_priv.regs = ((struct mgeth_regs*) nic->base_addr);

	/* Load current MAC address */
	memcpy(nic->dev_addr, hw_addr, ETH_ALEN);

	mgeth_netdev = nic;

	res = irq_attach(nic->irq, mgeth_irq_handler, 0, nic, "");
	if (res < 0) {
		return res;
	}

	//log_error("line %d", __LINE__);
	rcm_mgpio_init(MGPIO0_BASE);
	usleep(100000);
//	log_error("line %d", __LINE__);
	for(i = 0; i < 4; i ++) {
		rcm_mdio_init(i);
	}
	usleep(100000);
//	log_error("line %d", __LINE__);
	for(i = 0; i < 4; i ++) {
		rcm_mdio_en(i, AUTONEGOTIATION_EN);
	}
	usleep(100000);
//	log_error("line %d", __LINE__);
	phy_rcm_sgmii_init(AUTONEGOTIATION_EN);
	log_error("line %d", __LINE__);

	return inetdev_register_dev(nic);
}


STATIC_IRQ_ATTACH(MGETH_IRQ, mgeth_irq_handler, mgeth_netdev);

PERIPH_MEMORY_DEFINE(mgeth, BASE_ADDR, 0x1000);

PERIPH_MEMORY_DEFINE(mgpio0, MGPIO0_BASE, 0x2000);
