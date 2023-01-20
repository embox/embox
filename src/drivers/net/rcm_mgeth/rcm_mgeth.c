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

#include <embox/unit.h>

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


#define SGMII_PHY_BASE (0x01086000)
#define MGPIO0_BASE    (0x01084000)
#define MDIO0_BASE     (0x01080000)
#define SCTL_BASE     (0x0108D000)

#include "rcm_mgeth.h"
#include "rcm_mdio.h"

EMBOX_UNIT_INIT(mgeth_init);

#define BASE_ADDR   OPTION_GET(NUMBER, base_addr)
#define MGETH_IRQ   OPTION_GET(NUMBER, irq_num)


extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);




#define ETH_PHY_ID                0xBBCD

#define AN_EN          0


#define MGETH_VER 0x01900144

// RXBD_COUNT should be multiply of 4
#define MGETH_RXBD_CNT 256
#define MGETH_TXBD_CNT 1

#define MGETH_BD_POLL_ALIGN 0x1000




#define ARCH_DMA_MINALIGN	128

/* CONFIG_SYS_CACHELINE_SIZE is used a lot in drivers */
#define CONFIG_SYS_CACHELINE_SIZE	ARCH_DMA_MINALIGN



#define MGETH_MIN_PACKET_LEN  60
#define MGETH_MAX_PACKET_LEN  0x3fff

#define MGETH_RXBUF_SIZE      1540

#define MGETH_TX_TIMEOUT      5



struct mgeth_priv {
	mgeth_regs *regs;

	struct mdma_chan *tx_chan;
	struct mdma_chan *rx_chan;

	struct rcm_mgeth_long_desc *rxbd_base;
	struct rcm_mgeth_long_desc *txbd_base;

	int rxbd_no;

//	unsigned int speed;
//	unsigned int duplex;
//	unsigned int link;

	//unsigned char *buffer;

	unsigned char dev_addr[ETH_ALEN];
};



typedef struct{
  uint32_t length;
  uint32_t buffer;
}FrameTypeDef;

static struct mgeth_priv mgeth;

uint8_t rxbd[MGETH_RXBD_CNT * sizeof(struct rcm_mgeth_long_desc)] __attribute__ ((aligned (16))); // Intermediate buffer
uint8_t txbd[MGETH_TXBD_CNT * sizeof(struct rcm_mgeth_long_desc)] __attribute__ ((aligned (16))); // Intermediate buffer

uint8_t EthFrameRX[MGETH_RXBD_CNT - 1][MGETH_RXBUF_SIZE] __attribute__ ((aligned (8))); // Intermediate buffer
uint8_t EthFrameTX[MGETH_TXBD_CNT][MGETH_RXBUF_SIZE] __attribute__ ((aligned (8))); // Intermediate buffer

static int mgeth_xmit(struct net_device *dev, struct sk_buff *skb);
static int mgeth_open(struct net_device *dev);
static int mgeth_close(struct net_device *dev);
static int mgeth_set_mac(struct net_device *dev, const void *addr);



static void mgeth_init_descr(struct mgeth_priv *priv) {
	int i;

	/* Initiate rx decriptors */
	for (i = 0; i < (MGETH_RXBD_CNT - 1); i++) {
		priv->rxbd_base[i].usrdata_h = 0;
		priv->rxbd_base[i].usrdata_l = 0;
		/* enable descriptor */
		priv->rxbd_base[i].flags_length = MGETH_BD_INT | MGETH_RXBUF_SIZE; // usual descriptor
		priv->rxbd_base[i].memptr = (uint32_t) (uintptr_t)(&EthFrameRX[i][0]);

	}
	/* set wrap last to first descriptor */
	priv->rxbd_base[MGETH_RXBD_CNT - 1].flags_length = MGETH_BD_INT | MGETH_BD_LINK; // link descriptor
	priv->rxbd_base[MGETH_RXBD_CNT - 1].memptr = (uint32_t) (uintptr_t)&priv->rxbd_base[0];

	dcache_flush(priv->rxbd_base, MGETH_RXBD_CNT * sizeof(struct rcm_mgeth_long_desc));
	/* initiate indexes */
	priv->rxbd_no = 0;

	/* Initiate tx decriptor */
	priv->txbd_base[0].usrdata_h = 0;
	priv->txbd_base[0].usrdata_l = 0;
	priv->txbd_base[0].memptr = 0;
	priv->txbd_base[0].flags_length = MGETH_BD_STOP;
	dcache_flush(priv->txbd_base, MGETH_TXBD_CNT * sizeof(struct rcm_mgeth_long_desc));

	/* Set pointer to rx descriptor areas */
	REG32_STORE(&priv->regs->rx[0].settings,
			MGETH_CHAN_DESC_LONG | MGETH_CHAN_ADD_INFO | (sizeof(struct rcm_mgeth_long_desc) << MGETH_CHAN_DESC_GAP_SHIFT));
	REG32_STORE(&priv->regs->rx[0].desc_addr, (uint32_t )&priv->rxbd_base[0]);
}

static void mgeth_set_packet_filter(const mgeth_regs *regs, unsigned char dev_addr[6]) {

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


static inline void mgeth_reset_mac(const mgeth_regs *regs) {
	REG32_STORE(&regs->sw_rst, 0x1);

	while (REG32_LOAD(&regs->sw_rst) & 0x1) {
		;
	}
}

static inline void mgeth_init_mac(const mgeth_regs *regs, int speed, int duplex) {
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
static int mgeth_start(struct mgeth_priv *priv, uint8_t *mac) {
	mgeth_reset_mac(priv->regs);

	mgeth_init_mac(priv->regs, MGETH_SPEED_1000, 1);

	// disable interrupts
	REG32_STORE(&priv->regs->mg_irq_mask, 0);

	// set filters for given MAC
	mgeth_set_packet_filter(priv->regs, priv->dev_addr);

	// setup RX/TX queues
	mgeth_init_descr(priv);

	// 1.4.1.6.4.4.2.12	IRQ_MASK_R(W) (R – 0xA14, W – 0x314)
	// int_desc	2
	REG32_STORE(&priv->regs->rx[0].irq_mask, (1 << 2));

	// enable rx
	REG32_STORE(&priv->regs->rx[0].enable, MGETH_ENABLE);

	return 0;
}

/* Stop the hardware from looking for packets - may be called even if
 *	 state == PASSIVE
 */
static void mgeth_stop(struct mgeth_priv *priv) {
	REG32_STORE(&priv->regs->rx[0].enable, 0);
	REG32_STORE(&priv->regs->tx[0].enable, 0);
}

/* Send the bytes passed in "packet" as a packet on the wire */
static int mgeth_send(struct mgeth_priv *priv, struct sk_buff *skb) {
	struct _mgeth_tx_regs *mgeth_tx_regs = &priv->regs->tx[0];
	int len = skb->len;

	// copy to internal buffer if size less than min
	if (len < MGETH_MIN_PACKET_LEN) {
		memset(&EthFrameTX[0][0], 0, MGETH_MIN_PACKET_LEN);
		memcpy(&EthFrameTX[0][0], skb->mac.raw, len);
		len = MGETH_MIN_PACKET_LEN;
	} else {
		memcpy(&EthFrameTX[0][0], skb->mac.raw, len);
	}
	dcache_flush(&EthFrameTX[0][0], len);

	// wait for completion of previous transfer
	while ( (REG32_LOAD(&mgeth_tx_regs->enable) & MGETH_ENABLE) != 0) {
		;
	}


	priv->txbd_base[0].flags_length = MGETH_BD_STOP | len;
	priv->txbd_base[0].memptr = (uintptr_t) &EthFrameTX[0][0];
	dcache_flush(&priv->txbd_base[0], sizeof(priv->txbd_base[0]));

	// enable tx
	REG32_STORE(&mgeth_tx_regs->desc_addr, (uintptr_t)&priv->txbd_base[0]);
	REG32_STORE(&mgeth_tx_regs->enable, MGETH_ENABLE);

	return 0;
}

static FrameTypeDef mgeth_recv(struct mgeth_priv *priv, uint32_t descr) {
	struct rcm_mgeth_long_desc *curr_bd = &priv->rxbd_base[descr];
	FrameTypeDef frame = { 0, 0 };

	dcache_inval(curr_bd, sizeof(struct rcm_mgeth_long_desc));

	if (!(curr_bd->flags_length & MGETH_BD_OWN)) {
		return (frame);
	}

	frame.length = curr_bd->flags_length & MGETH_MAX_PACKET_LEN;
	frame.buffer = curr_bd->memptr;
	dcache_inval((void*)(uintptr_t)frame.buffer, frame.length);

	return (frame);
}

/* Give the driver an opportunity to manage its packet buffer memory
 *	     when the network stack is finished processing it. This will only be
 *	     called when no error was returned from recv
 */
static int mgeth_free_pkt(struct mgeth_priv *priv, uint32_t descr) {
	priv->rxbd_base[descr].usrdata_h = 0;
	priv->rxbd_base[descr].usrdata_l = 0;
	// enable desciptor & set wrap last to first descriptor
	if (descr >= (MGETH_RXBD_CNT - 1)) {
		priv->rxbd_base[descr].flags_length = MGETH_BD_INT | MGETH_BD_LINK; // link descriptor
		priv->rxbd_base[descr].memptr = (unsigned int) &priv->rxbd_base[0];
		dcache_flush(&priv->rxbd_base[descr], sizeof(struct rcm_mgeth_long_desc));
		//*((volatile uint32_t*)&priv->regs->rx[0].irq_mask) = (1 << 2);
		REG32_STORE(&priv->regs->rx[0].enable, MGETH_ENABLE);
	} else {
		priv->rxbd_base[descr].flags_length = MGETH_BD_INT | MGETH_RXBUF_SIZE; // usual descriptor
		priv->rxbd_base[descr].memptr = (unsigned int) (&EthFrameRX[descr][0]);
		dcache_flush(&priv->rxbd_base[descr], sizeof(struct rcm_mgeth_long_desc));
	}

	return 0;
}

static irq_return_t mgeth_irq_handler(unsigned int irq_num, void *dev_id) {
	struct net_device *nic_p = dev_id;
	uint32_t MDAM_status;
	FrameTypeDef frame;

	MDAM_status = REG32_LOAD(&mgeth.regs->rx[0].status);

	log_debug("irq status (0x%x)", MDAM_status);

	if (!(MDAM_status & (1 << 2))) {
		return IRQ_NONE;
	}

	if (!nic_p) {
		return IRQ_NONE;
	}

	irq_lock();
	for (mgeth.rxbd_no = 0; mgeth.rxbd_no < MGETH_RXBD_CNT; mgeth.rxbd_no++) {

		frame = mgeth_recv(&mgeth, mgeth.rxbd_no);

		if (frame.length > 0) {
			/* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
			struct sk_buff *skb = skb_alloc(frame.length);
			/* copy received frame to pbuf chain */
			if (skb != NULL) {
				memcpy(skb->mac.raw, (const void*) frame.buffer, frame.length);
				skb->dev = nic_p;
				show_packet(skb->mac.raw, skb->len, "rx");
				netif_rx(skb);
			} else {
				log_error("skb_alloc failed");
			}
		}

		mgeth_free_pkt(&mgeth, mgeth.rxbd_no);
	}
	irq_unlock();

	return IRQ_HANDLED;
}

static int mgeth_xmit(struct net_device *dev, struct sk_buff *skb) {
	if (dev == NULL) {
		return -EINVAL;
	}

	mgeth_send(&mgeth, skb);
	show_packet(skb->mac.raw, skb->len, "tx");
	skb_free(skb);

	return ENOERR;
}


static int mgeth_open(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}
	mgeth_start(&mgeth, (uint8_t*) &mgeth.dev_addr);

	return ENOERR;
}

static int mgeth_close(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}	
	mgeth_stop(&mgeth);

	return ENOERR;
}

static int mgeth_set_mac(struct net_device *dev, const void *addr) {
	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	memcpy(mgeth.dev_addr, addr, ETH_ALEN);

	mgeth_stop(&mgeth);
	mgeth_start(&mgeth, (uint8_t*) &mgeth.dev_addr);

	return ENOERR;
}

extern void rcm_mgpio_init(uint32_t mgpio_base);
extern int phy_rcm_sgmii_init(uint32_t sgmii_addr, uint32_t sctl_addr, int en);

static const struct net_driver mgeth_ops = {
		.xmit = mgeth_xmit,
		.start = mgeth_open,
		.stop = mgeth_close,
		.set_macaddr = mgeth_set_mac,
};

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

	nic->drv_ops = &mgeth_ops;
	nic->irq = MGETH_IRQ;
	nic->base_addr = BASE_ADDR;

	mgeth.regs = ((mgeth_regs*) nic->base_addr);
	mgeth.rxbd_base = (struct rcm_mgeth_long_desc*) &rxbd[0];
	mgeth.txbd_base = (struct rcm_mgeth_long_desc*) &txbd[0];

	/* Load current MAC address */
	memcpy(mgeth.dev_addr, hw_addr, ETH_ALEN);

#if 0
	memset(mgeth.rxbd_base, 0, MGETH_RXBD_CNT * sizeof(struct rcm_mgeth_long_desc));
	dcache_flush(mgeth.rxbd_base, MGETH_RXBD_CNT * sizeof(struct rcm_mgeth_long_desc));
	memset(mgeth.txbd_base, 0, MGETH_TXBD_CNT * sizeof(struct rcm_mgeth_long_desc));
	dcache_flush(mgeth.txbd_base, MGETH_TXBD_CNT * sizeof(struct rcm_mgeth_long_desc));
#endif
	mgeth_netdev = nic;

	res = irq_attach(nic->irq, mgeth_irq_handler, 0, nic, "");
	if (res < 0) {
		return res;
	}

	rcm_mgpio_init(MGPIO0_BASE);
	usleep(1000000);

	for(i = 0; i < 4; i ++) {
		rcm_mdio_init(MDIO0_BASE + 0x1000 * i);
	}
	usleep(1000000);

	for(i = 0; i < 4; i ++) {
		rcm_mdio_en(MDIO0_BASE + 0x1000 * i, AN_EN);
	}
	usleep(1000000);

	phy_rcm_sgmii_init(SGMII_PHY_BASE, SCTL_BASE, AN_EN);

	log_debug("line %d", __LINE__);

	return inetdev_register_dev(nic);
}


STATIC_IRQ_ATTACH(MGETH_IRQ, mgeth_irq_handler, mgeth_netdev);

PERIPH_MEMORY_DEFINE(mgeth, BASE_ADDR, 0x1000);

PERIPH_MEMORY_DEFINE(sgmii_phy, SGMII_PHY_BASE, 0x2000);

PERIPH_MEMORY_DEFINE(mgpio0, MGPIO0_BASE, 0x2000);

PERIPH_MEMORY_DEFINE(mdio0, MDIO0_BASE, 0x4000);

PERIPH_MEMORY_DEFINE(SCTL, SCTL_BASE, 0x1000);
