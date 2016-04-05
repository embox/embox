/**
 * @file
 * @brief Ethernet Media Access Controller for TMS320DM816x DaVinci
 *
 * @date 20.09.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <embox/unit.h>

#include <kernel/irq.h>
#include <hal/reg.h>
#include <drivers/net/ti816x.h>

#include <mem/misc/pool.h>

#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

#include <util/binalign.h>
#include <util/math.h>
#include <util/log.h>
#include <util/dlist.h>

#include <mem/misc/pool.h>

#include <framework/mod/options.h>

EMBOX_UNIT_INIT(ti816x_init);

#define MODOPS_PREP_BUFF_CNT	OPTION_GET(NUMBER, prep_buff_cnt)
#define MODOPS_AMOUNT_SKB	OPTION_GET(NUMBER, amount_skb)

#define DEFAULT_CHANNEL 0
#define DEFAULT_MASK ((uint8_t)(1 << DEFAULT_CHANNEL))

#define EMAC_SAFE_PADDING 64

#define RX_BUFF_LEN       0x800
#define RX_FRAME_MAX_LEN  \
	min(min(RX_BUFF_LEN, skb_max_size()), (ETH_FRAME_LEN + ETH_FCS_LEN))

struct emac_desc_head {
	char buf[EMAC_SAFE_PADDING];
	struct emac_desc desc;
	char data[RX_BUFF_LEN];
	struct sk_buff *skb;
} __attribute__ ((aligned (0x4)));

static struct emac_desc_head *head_from_desc(struct emac_desc *desc) {
	return (struct emac_desc_head *) (((char*)desc) - EMAC_SAFE_PADDING);
}

static struct emac_desc_head emac_rx_list[MODOPS_PREP_BUFF_CNT];
POOL_DEF(emac_tx_desc_pool, struct emac_desc_head, MODOPS_PREP_BUFF_CNT + 0x1);

static struct emac_desc_head *emac_hdesc_tx_alloc(void) {
	ipl_t sp;
	struct emac_desc_head *res;

	sp = ipl_save();
	{
		res = pool_alloc(&emac_tx_desc_pool);
		assert(res);
		memset(res, 0xFF, EMAC_SAFE_PADDING);
	}
	ipl_restore(sp);

	return res;
}

static void emac_hdesc_tx_free(struct emac_desc_head *obj) {
	ipl_t sp;
	int i;
	assert(obj);

	sp = ipl_save();
	{
		for (i = 0; i < EMAC_SAFE_PADDING; i++)
			assert(obj->buf[i] == 0xFF);

		pool_free(&emac_tx_desc_pool, obj);
	}
	ipl_restore(sp);
}

extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);

static void emac_desc_set_next(struct emac_desc *desc, void *next) {
	desc->next = (uintptr_t) next;
	dcache_flush(desc, sizeof(struct emac_desc));
}

struct ti816x_priv {
	struct emac_desc *rx_head;

	struct emac_desc *rx_wait_head;
	struct emac_desc *rx_wait_tail;

	struct emac_desc *tx_cur_head;

	struct emac_desc *tx_wait_head;
	struct emac_desc *tx_wait_tail;
	int tx_busy;
};

static void ti816x_config(struct net_device *dev);

static void emac_ctrl_enable_irq(void) {
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXTHRESHINTEN, 0xff);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXINTEN, 0xff);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMTXINTEN, 0xff);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMMISCINTEN, 0xf);
}

static void emac_ctrl_disable_irq(void) {
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXTHRESHINTEN, 0x0);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXINTEN, 0x0);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMTXINTEN, 0x0);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMMISCINTEN, 0x0);
}

#define C0_TX (0x1 << 17) /* CMINTCTRL */
#define C0_RX (0x1 << 16)
#define INTPRESCALE(x) ((x & 0x7ff) << 0)
#define RXIMAX(x) ((x & 0x3f) << 0) /* CMRXINTMAX */
#define TXIMAX(x) ((x & 0x3f) << 0) /* CMTXINTMAX */
static inline void emac_ctrl_enable_pacing(void) {
	/* enable rx/tx pacing; set pacing period */
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMINTCTRL,
			C0_RX | C0_TX | INTPRESCALE(125 * 4));

	/* set maximum number of rx/tx interrupts */
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXINTMAX, RXIMAX(4));
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMTXINTMAX, TXIMAX(4));
}

static void cm_load_mac(struct net_device *dev) {
	unsigned long mac_hi, mac_lo;

	mac_hi = REG_LOAD(CM_BASE + CM_R_MACID0_HI);
	mac_lo = REG_LOAD(CM_BASE + CM_R_MACID0_LO);

	assert(dev != NULL);
	dev->dev_addr[0] = mac_hi & 0xff;
	dev->dev_addr[1] = (mac_hi >> 8) & 0xff;
	dev->dev_addr[2] = (mac_hi >> 16) & 0xff;
	dev->dev_addr[3] = mac_hi >> 24;
	dev->dev_addr[4] = mac_lo & 0xff;
	dev->dev_addr[5] = mac_lo >> 8;
}

static void emac_reset(void) {
	REG_STORE(EMAC_BASE + EMAC_R_SOFTRESET, 1);
	while (REG_LOAD(EMAC_BASE + EMAC_R_SOFTRESET) & 1);
}

static void emac_clear_hdp(void) {
	int i;

	for (i = 0; i < EMAC_CHANNEL_COUNT; ++i) {
		REG_STORE(EMAC_BASE + EMAC_R_TXHDP(i), 0);
		REG_STORE(EMAC_BASE + EMAC_R_RXHDP(i), 0);
	}
}

static void emac_clear_ctrl_regs(void) {
	REG_STORE(EMAC_BASE + EMAC_R_TXCONTROL, 0);
	REG_STORE(EMAC_BASE + EMAC_R_RXCONTROL, 0);
	REG_STORE(EMAC_BASE + EMAC_R_MACCONTROL, 0);
}

static void emac_set_stat_regs(unsigned long v) {
	int i;

	for (i = 0; i < 36; ++i) {
		REG_STORE(EMAC_BASE + 0x200 + 0x4 * i, v);
	}
}

#define MACINDEX(x) ((x & 0x1f) << 0)
#define VALID (0x1 << 20)
#define MATCHFILT (0x1 << 19)
#define CHANNEL(x) ((x & 0x7) << 16)
static void emac_set_macaddr(unsigned char (*addr)[6]) {
	int i;
	unsigned long mac_hi, mac_lo;

	mac_hi = ((*addr)[3] << 24) | ((*addr)[2] << 16)
			| ((*addr)[1] << 8) | ((*addr)[0] << 0);
	mac_lo = ((*addr)[5] << 8) | ((*addr)[4] << 0);

	for (i = 0; i < EMAC_CHANNEL_COUNT; ++i) {
		REG_STORE(EMAC_BASE + EMAC_R_MACINDEX, MACINDEX(i));
		REG_STORE(EMAC_BASE + EMAC_R_MACADDRHI, mac_hi);
		REG_STORE(EMAC_BASE + EMAC_R_MACADDRLO,
				mac_lo | VALID | MATCHFILT | CHANNEL(i));
	}

	REG_STORE(EMAC_BASE + EMAC_R_MACSRCADDRHI, mac_hi);
	REG_STORE(EMAC_BASE + EMAC_R_MACSRCADDRLO, mac_lo);
}

static void emac_init_rx_regs(void) {
	int i;

	/* disable filtering */
	REG_STORE(EMAC_BASE + EMAC_R_RXFILTERLOWTHRESH, 0);

	for (i = 0; i < EMAC_CHANNEL_COUNT; ++i) {
		REG_STORE(EMAC_BASE + EMAC_R_RXFLOWTHRESH(i), 0);
		REG_STORE(EMAC_BASE + EMAC_R_RXFREEBUFFER(i), MODOPS_PREP_BUFF_CNT);
	}
}

static void emac_clear_machash(void) {
	REG_STORE(EMAC_BASE + EMAC_R_MACHASH1, 0);
	REG_STORE(EMAC_BASE + EMAC_R_MACHASH2, 0);
}

static void emac_set_rxbufoff(unsigned long v) {
	REG_STORE(EMAC_BASE + EMAC_R_RXBUFFEROFFSET, v);
}

static void emac_clear_and_enable_rxunicast(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXUNICASTCLEAR, 0xff);
	REG_STORE(EMAC_BASE + EMAC_R_RXUNICASTSET, 0xff);
}

#define RXNOCHAIN (0x1 << 28) /* single buffer */
#define RXCMFEN (0x1 << 24) /* short msg */
#define RXCSFEN (0x1 << 23) /* short msg */
#define RXCEFEN (0x1 << 22) /* short msg */
#define RXCAFEN (0x1 << 21) /* promiscuous */
#define RXBROADEN (0x1 << 13) /* broadcast */
#define RXMULTEN (0x1 << 5) /* multicast */
static void emac_enable_rxmbp(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXMBPENABLE, RXMBP_INIT);
}

#define GIG_FORCE (0x1 << 17)
#define GIG (0x1 << 7)
#define TXPACE (0x1 << 6)
#define GMIIEN (0x1 << 5)
#define FULLDUPLEX (0x1 << 0)
static void emac_set_macctrl(unsigned long v) {
	REG_ORIN(EMAC_BASE + EMAC_R_MACCONTROL, v);
}

#define HOSTMASK (0x1 << 1)
#define STATMASK (0x1 << 0)
static void emac_enable_rx_and_tx_irq(void) {
	/* mask unused channel */
	REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKCLEAR, ~DEFAULT_MASK);
	REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKCLEAR, ~DEFAULT_MASK);

	/* allow all rx and tx interrupts */
	REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKSET, DEFAULT_MASK);
	REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKSET, DEFAULT_MASK);

	/* allow host error and statistic interrupt */
	REG_STORE(EMAC_BASE + EMAC_R_MACINTMASKSET, HOSTMASK | STATMASK);
}

#define RXEN (0x1 << 0)
#define TXEN (0x1 << 0)
static void emac_enable_rx_and_tx_dma(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXCONTROL, RXEN);
	REG_STORE(EMAC_BASE + EMAC_R_TXCONTROL, TXEN);
}

static void emac_desc_build(struct emac_desc_head *hdesc, struct sk_buff *skb,
		size_t data_len, size_t packet_len, int flags) {
	assert(hdesc != NULL);
	assert(binalign_check_bound((uintptr_t)&hdesc->desc, 4));
	assert(data_len != 0);
	assert(flags & EMAC_DESC_F_OWNER);

	hdesc->desc.next = 0;
	hdesc->desc.data = (uintptr_t)(skb ? skb_data_cast_in(skb->data) : &hdesc->data[0]);
	hdesc->desc.data_len = data_len;
	hdesc->desc.packet_len = packet_len;
	hdesc->desc.data_off = 0;
	hdesc->desc.flags = flags;
	hdesc->skb = skb;
	dcache_flush(&hdesc->desc, sizeof hdesc->desc);
}

static int emac_desc_confirm(struct emac_desc *desc,
		unsigned long reg_cp) {
	REG_STORE(EMAC_BASE + reg_cp, (uintptr_t)desc);
	return (uintptr_t)desc != REG_LOAD(EMAC_BASE + reg_cp);
}

static void emac_queue_activate(struct emac_desc *desc,
		unsigned long reg_hdp) {
	assert(desc != NULL);
	REG_STORE(EMAC_BASE + reg_hdp, (uintptr_t)desc);
	dcache_flush((void*)(EMAC_BASE + reg_hdp), sizeof (uintptr_t));
}

static void emac_alloc_rx_queue(struct ti816x_priv *dev_priv) {
	int i;
	assert(dev_priv);

	dev_priv->rx_head = &emac_rx_list[0].desc;
	dev_priv->rx_wait_head = NULL;
	dev_priv->rx_wait_tail = NULL;

	emac_desc_build(&emac_rx_list[0], 0, RX_BUFF_LEN, 0, EMAC_DESC_F_OWNER);

	for (i = 1; i < MODOPS_PREP_BUFF_CNT; i++) {
		emac_desc_build(&emac_rx_list[i], 0, RX_BUFF_LEN, 0, EMAC_DESC_F_OWNER);
		emac_desc_set_next(	&emac_rx_list[i - 1].desc, &emac_rx_list[i].desc);
	}

	emac_queue_activate(dev_priv->rx_head, EMAC_R_RXHDP(DEFAULT_CHANNEL));
}

static int ti816x_xmit(struct net_device *dev, struct sk_buff *skb) {
	size_t data_len;
	struct emac_desc_head *hdesc;
	struct emac_desc *desc;
	struct ti816x_priv *dev_priv;
	ipl_t ipl;

	hdesc = emac_hdesc_tx_alloc();
	desc = &hdesc->desc;

	data_len = max(skb->len, ETH_ZLEN);
	dcache_flush(skb_data_cast_in(skb->data), data_len);

	emac_desc_build(hdesc, skb, data_len, data_len,
			EMAC_DESC_F_SOP | EMAC_DESC_F_EOP | EMAC_DESC_F_OWNER);

	dev_priv = netdev_priv(dev, struct ti816x_priv);
	assert(dev_priv != NULL);

	ipl = ipl_save();
	{
		if (dev_priv->tx_cur_head) {
			/* Add packet to the waiting queue */
			if (dev_priv->tx_wait_head) {
				emac_desc_set_next(dev_priv->tx_wait_head, desc);
			} else {
				dev_priv->tx_wait_head = desc;
			}
			dev_priv->tx_wait_tail = desc;
		} else {
			/* Process packet immediately */
			dev_priv->tx_cur_head = desc;
			emac_queue_activate(desc, EMAC_R_TXHDP(DEFAULT_CHANNEL));
		}
	}
	ipl_restore(ipl);

	return 0;
}

static int ti816x_set_macaddr(struct net_device *dev, const void *addr) {
	emac_set_macaddr((unsigned char (*)[6])addr);
	return 0;
}

static int ti816x_open(struct net_device *dev) {
	emac_ctrl_enable_irq();
	return 0;
}

static int ti816x_stop(struct net_device *dev) {
	emac_ctrl_disable_irq();
	return 0;
}

static const struct net_driver ti816x_ops = {
	.xmit = ti816x_xmit,
	.start = ti816x_open,
	.stop = ti816x_stop,
	.set_macaddr = ti816x_set_macaddr
};

#define RXTHRESHEOI 0x0 /* MACEOIVECTOR */
static irq_return_t ti816x_interrupt_macrxthr0(unsigned int irq_num,
		void *dev_id) {
	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE
				+ EMAC_R_CMRXTHRESHINTSTAT));

	log_debug("ti816x_interrupt_macrxthr0: unhandled interrupt\n");

	REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, RXTHRESHEOI);

	return IRQ_HANDLED;
}

#define CHECK_RXOK(x) \
	(!(~(x) & (EMAC_DESC_F_SOP | EMAC_DESC_F_EOP)))
#define CHECK_RXERR(x) \
	((x) & (EMAC_DESC_F_OWNER | EMAC_DESC_F_TDOWNCMPLT       \
			| EMAC_DESC_F_JABBER       \
			| EMAC_DESC_F_OVERSIZE | EMAC_DESC_F_FRAGMENT    \
			| EMAC_DESC_F_UNDERSIZED | EMAC_DESC_F_CONTROL   \
			| EMAC_DESC_F_OVERRUN | EMAC_DESC_F_CODEERROR    \
			| EMAC_DESC_F_ALIGNERROR | EMAC_DESC_F_CRCERROR))
#define RXEOI 0x1 /* MACEOIVECTOR */
#define CHECK_RXERR_2(x) \
	((x) & (EMAC_DESC_F_OWNER | EMAC_DESC_F_TDOWNCMPLT       \
			| EMAC_DESC_F_JABBER       \
			| EMAC_DESC_F_OVERSIZE | EMAC_DESC_F_FRAGMENT    \
			| EMAC_DESC_F_UNDERSIZED | EMAC_DESC_F_CONTROL   \
			| EMAC_DESC_F_OVERRUN | EMAC_DESC_F_CODEERROR    \
			| EMAC_DESC_F_ALIGNERROR))

static irq_return_t ti816x_interrupt_macrxint0(unsigned int irq_num,
		void *dev_id) {
	struct ti816x_priv *dev_priv;
	int eoq;
	struct emac_desc *desc;
	struct emac_desc_head *hdesc;
	struct sk_buff *skb;
	log_debug("ti rx int");

	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE
				+ EMAC_R_CMRXINTSTAT));

	dev_priv = netdev_priv(dev_id, struct ti816x_priv);
	assert(dev_priv != NULL);

	desc = dev_priv->rx_head;

	eoq = 0;
	do {
		dcache_inval(desc, sizeof (struct emac_desc));

		log_debug("desc %#x", (unsigned int)desc);
		if (desc->flags & EMAC_DESC_F_OWNER) {
			log_info("ti rx break\n");
			break;
		}
		emac_desc_confirm(desc, EMAC_R_RXCP(DEFAULT_CHANNEL));

		hdesc = head_from_desc(desc);
		if (desc->flags & EMAC_DESC_F_PASSCRC) {
			desc->data_len -= 4;
		}

		assert(!(desc->flags & EMAC_DESC_F_TDOWNCMPLT));
		assert(!hdesc->skb); /* We do not prealloc skb for RX queue */

		if (!CHECK_RXERR_2(desc->flags)) {
			if (desc->data_len > RX_FRAME_MAX_LEN) {
				log_debug("<too long frame %x>", desc->flags);
			} else {
				dcache_inval((void*)desc->data, desc->data_len);

				skb = skb_alloc(desc->data_len);
				assert(skb);
				skb->len = desc->data_len;
				skb->dev = dev_id;
				memcpy(skb_data_cast_in(skb->data),
					(void*)desc->data, desc->data_len);
				netif_rx(skb);
			}
		} else {
			log_debug("<ASSERT %x>", desc->flags);
		}

		eoq = desc->flags & EMAC_DESC_F_EOQ;
		desc = (void*) desc->next;
		dev_priv->rx_head = desc;

		log_debug("reuse %#x", &hdesc->desc);
		emac_desc_build(hdesc, 0, RX_BUFF_LEN, 0, EMAC_DESC_F_OWNER);
		if (!dev_priv->rx_wait_head) {
			dev_priv->rx_wait_head = &hdesc->desc;
		} else {
			emac_desc_set_next(dev_priv->rx_wait_tail, &hdesc->desc);
		}
		dev_priv->rx_wait_tail = &hdesc->desc;

		if (eoq) {
			assert(!desc);
		} else {
			assert(desc);
		}
	} while (!eoq);

	if (eoq) {
		log_debug("update rx queue");
		dev_priv->rx_head = dev_priv->rx_wait_head;
		emac_queue_activate(dev_priv->rx_head, EMAC_R_RXHDP(DEFAULT_CHANNEL));
	}

	REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, RXEOI);

	return IRQ_HANDLED;
}

#define CHECK_TXOK(x) 1
#define CHECK_TXERR(x) \
	((x) & (EMAC_DESC_F_OWNER | EMAC_DESC_F_TDOWNCMPLT))
#define TXEOI 0x2 /* MACEOIVECTOR */
static irq_return_t ti816x_interrupt_mactxint0(unsigned int irq_num,
		void *dev_id) {
	struct ti816x_priv *dev_priv;
	int eoq;
	ipl_t sp;
	struct emac_desc *desc;
	struct emac_desc_head *hdesc;

	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE + EMAC_R_CMTXINTSTAT));
	log_debug("ti tx int");

	dev_priv = netdev_priv(dev_id, struct ti816x_priv);
	assert(dev_priv != NULL);

	desc = dev_priv->tx_cur_head;

	do {
		hdesc = head_from_desc(desc);
		dcache_inval(desc, sizeof *desc);
		assert(CHECK_TXOK(desc->flags));
		assert(!CHECK_TXERR(desc->flags));

		eoq = desc->flags & EMAC_DESC_F_EOQ;
		emac_desc_confirm(desc, EMAC_R_TXCP(DEFAULT_CHANNEL));

		desc = (void*) desc->next;

		skb_free(hdesc->skb);
		emac_hdesc_tx_free(hdesc);

	} while (!eoq && desc);

	assert(eoq && !desc);

	sp = ipl_save();
	{
		dev_priv->tx_cur_head = dev_priv->tx_wait_head;

		if (dev_priv->tx_cur_head) {
			emac_queue_activate(dev_priv->tx_cur_head,
					EMAC_R_TXHDP(DEFAULT_CHANNEL));
			dev_priv->tx_wait_head = dev_priv->tx_wait_tail = NULL;
		}
	}
	ipl_restore(sp);

	REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, TXEOI);

	return IRQ_HANDLED;
}

#define STATPEND (0x1 << 27) /* MACINVECTOR */
#define HOSTPEND (0x1 << 26)
#define TXPEND (DEFAULT_MASK << 16)
#define RXPEND (DEFAULT_MASK << 0)
#define IDLE(x) ((x >> 31) & 0x1) /* MACSTATUS */
#define TXERRCODE(x) ((x >> 20) & 0xf)
#define TXERRCH(x) ((x >> 16) & 0x7)
#define RXERRCODE(x) ((x >> 12) & 0xf)
#define RXERRCH(x) ((x >> 8) & 0x7)
#define RGMIIGIG(x) ((x >> 4) & 0x1)
#define RGMIIFULLDUPLEX(x) ((x >> 3) & 0x1)
#define RXQOSACT(x) ((x >> 2) & 0x1)
#define RXFLOWACT(x) ((x >> 1) & 0x1)
#define TXFLOWACT(x) ((x >> 0) & 0x1)
#define MISCEOI 0x3 /* MACEOIVECTOR */
static irq_return_t ti816x_interrupt_macmisc0(unsigned int irq_num,
		void *dev_id) {
	unsigned long macinvector;

	macinvector = REG_LOAD(EMAC_BASE + EMAC_R_MACINVECTOR);

	if (macinvector & STATPEND) {
		emac_set_stat_regs(0xffffffff);
		macinvector &= ~STATPEND;
	}
	if (macinvector & HOSTPEND) {
		unsigned long macstatus;

		macstatus = REG_LOAD(EMAC_BASE + EMAC_R_MACSTATUS);
		log_debug("\tMACSTATUS: %#lx\n"
				"\t\tidle %lx\n"
				"\t\ttxerrcode %lx; txerrch %lx\n"
				"\t\trxerrcode %lx; rxerrch %lx\n"
				"\t\trgmiigig %lx; rgmiifullduplex %lx\n"
				"\t\trxqosact %lx; rxflowact %lx; txflowact %lx\n",
				macstatus,
				IDLE(macstatus),
				TXERRCODE(macstatus), TXERRCH(macstatus),
				RXERRCODE(macstatus), RXERRCH(macstatus),
				RGMIIGIG(macstatus), RGMIIFULLDUPLEX(macstatus),
				RXQOSACT(macstatus), RXFLOWACT(macstatus), TXFLOWACT(macstatus));

		emac_reset();
		ti816x_config((struct net_device *)dev_id);

		macinvector &= ~HOSTPEND;
	}
	if (macinvector & (RXPEND | TXPEND)) {
		/* umm.. ok */
		macinvector &= ~(RXPEND | TXPEND);
	}
	if (macinvector) {
		log_debug("ti816x_interrupt_macmisc0: unhandled interrupt\n"
				"\tMACINVECTOR: %#lx\n"
				"\tCMMISCINTSTAT: %#lx\n",
				macinvector,
				REG_LOAD(EMAC_CTRL_BASE + EMAC_R_CMMISCINTSTAT));
	}

	REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, MISCEOI);

	return IRQ_HANDLED;
}

static void emac_set_max_frame_len(int m) {
	REG_STORE(EMAC_BASE + EMAC_R_RXMAXLEN, m);
}

static void ti816x_config(struct net_device *dev) {
	unsigned char bcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	/* reset EMAC module */
	emac_reset();

	/* disable all the EMAC/MDIO interrupts in the control module */
	emac_ctrl_disable_irq();

	/* initialization of the EMAC control module */
	emac_ctrl_enable_pacing();

	/* load device MAC-address */
	cm_load_mac(dev);

	/* initialization of EMAC and MDIO modules */
	emac_mdio_config();
	emac_clear_ctrl_regs();
	emac_clear_hdp();
	emac_set_stat_regs(0);
	emac_set_macaddr(&bcast_addr);
	emac_init_rx_regs();
	emac_clear_machash();
	emac_set_rxbufoff(0);
	emac_set_max_frame_len(RX_FRAME_MAX_LEN);
	emac_clear_and_enable_rxunicast();
	emac_enable_rxmbp();
	emac_set_macctrl(MACCTRL_INIT);
	emac_alloc_rx_queue(netdev_priv(dev, struct ti816x_priv));
	emac_enable_rx_and_tx_irq();
	emac_enable_rx_and_tx_dma();
	emac_set_macctrl(GMIIEN);

	/* enable all the EMAC/MDIO interrupts in the control module */
	/* emac_ctrl_enable_irq(); -- would done when opening */
}

static int ti816x_init(void) {
	int ret;
	struct net_device *nic;
	struct ti816x_priv *nic_priv;

	nic = etherdev_alloc(sizeof *nic_priv);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &ti816x_ops;
	nic->irq = nic->base_addr = 0;

	nic_priv = netdev_priv(nic, struct ti816x_priv);
	assert(nic_priv != NULL);
	memset(nic_priv, 0, sizeof *nic_priv);

	ti816x_config(nic);

	ret = irq_attach(MACRXTHR0, ti816x_interrupt_macrxthr0, 0,
			nic, "ti816x");
	if (ret < 0) {
		return ret;
	}
	ret = irq_attach(MACRXINT0, ti816x_interrupt_macrxint0, 0,
			nic, "ti816x");
	if (ret < 0) {
		return ret;
	}
	ret = irq_attach(MACTXINT0, ti816x_interrupt_mactxint0, 0,
			nic, "ti816x");
	if (ret < 0) {
		return ret;
	}
	ret = irq_attach(MACMISC0, ti816x_interrupt_macmisc0, 0,
			nic, "ti816x");
	if (ret < 0) {
		return ret;
	}

	return inetdev_register_dev(nic);
}
