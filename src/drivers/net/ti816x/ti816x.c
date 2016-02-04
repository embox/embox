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
#include <net/l0/net_entry.h>

#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <util/binalign.h>
#include <util/math.h>
#include <framework/mod/options.h>

#include <kernel/printk.h>

EMBOX_UNIT_INIT(ti816x_init);

#define MODOPS_PREP_BUFF_CNT OPTION_GET(NUMBER, prep_buff_cnt)
#define DEFAULT_CHANNEL 0
#define DEFAULT_MASK ((uint8_t)(1 << DEFAULT_CHANNEL))

struct emac_desc_head {
	struct emac_desc desc;
};

struct emac_desc_list {
	struct emac_desc_head *head;
	struct emac_desc_head *tail;
};

struct emac_desc_queue {
	struct emac_desc_list active;
	struct emac_desc_list pending;
};

struct ti816x_priv {
	struct emac_desc_queue rx;
	struct emac_desc_queue tx;
};

/* FIXME */
#include <module/embox/arch/mmu.h>
#ifndef NOMMU
extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);
#else
static inline void dcache_inval(const void *p, size_t size) { }
static inline void dcache_flush(const void *p, size_t size) { }
#endif

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
#define RXCSFEN (0x1 << 23) /* short msg */
#define RXCAFEN (0x1 << 21) /* promiscuous */
#define RXBROADEN (0x1 << 13) /* broadcast */
#define RXMULTEN (0x1 << 5) /* multicast */
static void emac_enable_rxmbp(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXMBPENABLE,
			RXNOCHAIN | RXCSFEN | RXCAFEN | RXBROADEN | RXMULTEN);
}

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

static void emac_desc_build(struct emac_desc_head *hdesc,
		void *data, size_t data_len, size_t len, int flags) {
	assert(hdesc != NULL);
	assert(binalign_check_bound((uintptr_t)hdesc, 4));
	assert(data != NULL);
	assert(data_len != 0);
	assert(flags & EMAC_DESC_F_OWNER);

	hdesc->desc.next = 0; /* use emac_desc_set_next */
	hdesc->desc.data = (uintptr_t)data;
	hdesc->desc.data_len = data_len;
	hdesc->desc.len = len;
	hdesc->desc.data_off = 0;
	hdesc->desc.flags = flags;
	dcache_flush(&hdesc->desc, sizeof hdesc->desc);
}

static void emac_desc_set_next(struct emac_desc_head *hdesc,
		struct emac_desc *next) {
	assert(hdesc != NULL);
	assert(next != NULL);
	assert(binalign_check_bound((uintptr_t)next, 4));

	hdesc->desc.next = (uintptr_t)next;
	dcache_flush(&hdesc->desc.next, sizeof hdesc->desc.next);
}

static int emac_desc_confirm(struct emac_desc_head *hdesc,
		unsigned long reg_cp) {
	REG_STORE(EMAC_BASE + reg_cp, (uintptr_t)&hdesc->desc);
	return (uintptr_t)&hdesc->desc != REG_LOAD(EMAC_BASE + reg_cp);
}

static void emac_list_add(struct emac_desc_list *ldesc,
		struct emac_desc_head *new_head,
		struct emac_desc_head *new_tail) {
	struct emac_desc_head *old_tail;

	assert(ldesc != NULL);
	assert(new_head != NULL);
	assert(new_tail != NULL);

	old_tail = ldesc->tail;
	ldesc->tail = new_tail;

	if (old_tail != NULL) {
		assert(ldesc->head != NULL);
		emac_desc_set_next(old_tail, &new_head->desc);
	}
	else {
		assert(ldesc->head == NULL);
		ldesc->head = new_head;
	}
}

static void emac_queue_add(struct emac_desc_queue *qdesc,
		struct emac_desc_head *hhead,
		struct emac_desc_head *htail) {
	assert(qdesc != NULL);
	emac_list_add(&qdesc->pending, hhead, htail);
}

static void emac_queue_reserve(struct emac_desc_queue *qdesc,
		size_t size) {
	size_t i;
	struct emac_desc_head *hdesc, *hfirst, *hprev;
	struct sk_buff_extra *skb_extra;
	struct sk_buff_data *skb_data;

	assert(qdesc != NULL);
	assert(size != 0);

	hfirst = hdesc = NULL;

	for (i = 0; i < size; ++i) {
		skb_extra = skb_extra_alloc();
		assert(skb_extra != NULL);
		skb_data = skb_data_alloc();
		assert(skb_data != NULL);

		hprev = hdesc;
		hdesc = skb_extra_cast_in(skb_extra);
		emac_desc_build(hdesc, skb_data_cast_in(skb_data),
				skb_max_size(), 0, EMAC_DESC_F_OWNER);

		if (hfirst == NULL) hfirst = hdesc;
		if (hprev != NULL) {
			emac_desc_set_next(hprev, &hdesc->desc);
		}
	}

	emac_list_add(&qdesc->active, hfirst, hdesc);
}

static void emac_queue_prepare(struct emac_desc_queue *qdesc) {
	assert(qdesc != NULL);
	assert((qdesc->active.head == NULL)
			&& (qdesc->active.tail == NULL));
	qdesc->active.head = qdesc->pending.head;
	qdesc->active.tail = qdesc->pending.tail;
	qdesc->pending.head = qdesc->pending.tail = NULL;
}

static void emac_queue_activate(struct emac_desc_queue *qdesc,
		unsigned long reg_hdp) {
	assert(qdesc != NULL);

	if (qdesc->active.head != NULL) {
		REG_STORE(EMAC_BASE + reg_hdp,
				(uintptr_t)&qdesc->active.head->desc);
	}
}

static void emac_queue_update(struct emac_desc_queue *qdesc,
		struct emac_desc_head *hdesc) {
	assert(qdesc != NULL);

	if (hdesc != NULL) {
		qdesc->active.head = hdesc;
	}
	else {
		qdesc->active.head = qdesc->active.tail = NULL;
	}
}

static struct emac_desc_head * emac_queue_head(
		struct emac_desc_queue *qdesc) {
	assert(qdesc != NULL);
	return qdesc->active.head;
}

static void emac_alloc_rx_queue(int size,
		struct ti816x_priv *dev_priv) {
	emac_queue_reserve(&dev_priv->rx, size);
	emac_queue_activate(&dev_priv->rx,
				EMAC_R_RXHDP(DEFAULT_CHANNEL));
}

static int ti816x_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct sk_buff_extra *skb_extra;
	struct sk_buff_data *skb_data;
	void *data;
	size_t data_len;
	struct emac_desc_head *hdesc;
	struct ti816x_priv *dev_priv;
	ipl_t ipl;

	skb_extra = skb_extra_alloc();
	assert(skb_extra != NULL);
	skb_data = skb_data_clone(skb->data);
	assert(skb_data != NULL);

	data = skb_data_cast_in(skb_data);
	data_len = max(skb->len, ETH_ZLEN);
	dcache_flush(data, data_len);

	skb_free(skb);

	hdesc = skb_extra_cast_in(skb_extra);
	emac_desc_build(hdesc, data, data_len, data_len,
			EMAC_DESC_F_SOP | EMAC_DESC_F_EOP | EMAC_DESC_F_OWNER);

	dev_priv = netdev_priv(dev, struct ti816x_priv);
	assert(dev_priv != NULL);

	ipl = ipl_save();
	{
		emac_queue_add(&dev_priv->tx, hdesc, hdesc);

		if (NULL == emac_queue_head(&dev_priv->tx)) {
			emac_queue_prepare(&dev_priv->tx);
			emac_queue_activate(&dev_priv->tx,
					EMAC_R_TXHDP(DEFAULT_CHANNEL));
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

	printk("ti816x_interrupt_macrxthr0: unhandled interrupt\n");

	REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, RXTHRESHEOI);

	return IRQ_HANDLED;
}

#define CHECK_RXOK(x) \
	(!(~(x) & (EMAC_DESC_F_SOP | EMAC_DESC_F_EOP)))
#define CHECK_RXERR(x) \
	((x) & (EMAC_DESC_F_OWNER | EMAC_DESC_F_TDOWNCMPLT       \
			| EMAC_DESC_F_PASSCRC | EMAC_DESC_F_JABBER       \
			| EMAC_DESC_F_OVERSIZE | EMAC_DESC_F_FRAGMENT    \
			| EMAC_DESC_F_UNDERSIZED | EMAC_DESC_F_CONTROL   \
			| EMAC_DESC_F_OVERRUN | EMAC_DESC_F_CODEERROR    \
			| EMAC_DESC_F_ALIGNERROR | EMAC_DESC_F_CRCERROR))
#define RXEOI 0x1 /* MACEOIVECTOR */
static irq_return_t ti816x_interrupt_macrxint0(unsigned int irq_num,
		void *dev_id) {
	struct ti816x_priv *dev_priv;
	void *data;
	int need_alloc, eoq;
	struct emac_desc_head *hdesc, *hnext;
	struct sk_buff *skb;

	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE
				+ EMAC_R_CMRXINTSTAT));

	dev_priv = netdev_priv(dev_id, struct ti816x_priv);
	assert(dev_priv != NULL);

	hnext = emac_queue_head(&dev_priv->rx);
	need_alloc = eoq = 0;

	do {
		assert(!eoq);

		hdesc = hnext;
		assert(hdesc != NULL);

		dcache_inval(&hdesc->desc, sizeof hdesc->desc);
		assert(CHECK_RXOK(hdesc->desc.flags));
		assert(!CHECK_RXERR(hdesc->desc.flags));

		data = (void *)(uintptr_t)hdesc->desc.data;
		dcache_inval(data, hdesc->desc.len);

		hnext = (struct emac_desc_head *)(uintptr_t)hdesc->desc.next;
		eoq = hdesc->desc.flags & EMAC_DESC_F_EOQ;

		skb = skb_wrap(hdesc->desc.len, skb_data_cast_out(data));
		if (skb == NULL) {
			skb_data_free(skb_data_cast_out(data));
		}
		else {
			skb->dev = dev_id;
			netif_rx(skb);
		}

		++need_alloc;
		skb_extra_free(skb_extra_cast_out(hdesc));
	} while (emac_desc_confirm(hdesc,
				EMAC_R_RXCP(DEFAULT_CHANNEL)));

	assert((NULL != hnext) || eoq);

	emac_queue_update(&dev_priv->rx, hnext);
	emac_queue_reserve(&dev_priv->rx, need_alloc);

	if (eoq) {
		emac_queue_activate(&dev_priv->rx,
				EMAC_R_RXHDP(DEFAULT_CHANNEL));
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
	void *data;
	int eoq;
	struct emac_desc_head *hdesc, *hnext;

	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE
				+ EMAC_R_CMTXINTSTAT));

	dev_priv = netdev_priv(dev_id, struct ti816x_priv);
	assert(dev_priv != NULL);

	hnext = emac_queue_head(&dev_priv->tx);
	eoq = 0;

	do {
		assert(!eoq);

		hdesc = hnext;
		assert(hdesc != NULL);

		dcache_inval(&hdesc->desc, sizeof hdesc->desc);
		assert(CHECK_TXOK(hdesc->desc.flags));
		assert(!CHECK_TXERR(hdesc->desc.flags));

		hnext = (struct emac_desc_head *)(uintptr_t)hdesc->desc.next;
		data = (void *)(uintptr_t)hdesc->desc.data;
		eoq = hdesc->desc.flags & EMAC_DESC_F_EOQ;

		skb_extra_free(skb_extra_cast_out(hdesc));
		skb_data_free(skb_data_cast_out(data));
	} while (emac_desc_confirm(hdesc,
				EMAC_R_TXCP(DEFAULT_CHANNEL)));

	assert((NULL != hnext) || eoq);

	emac_queue_update(&dev_priv->tx, hnext);
	if (NULL == emac_queue_head(&dev_priv->tx)) {
		emac_queue_prepare(&dev_priv->tx);
	}

	if (eoq) {
		emac_queue_activate(&dev_priv->tx,
				EMAC_R_TXHDP(DEFAULT_CHANNEL));
	}

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
		printk("\tMACSTATUS: %#lx\n"
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
		macinvector &= ~HOSTPEND;
	}
	if (macinvector & (RXPEND | TXPEND)) {
		/* umm.. ok */
		macinvector &= ~(RXPEND | TXPEND);
	}
	if (macinvector) {
		printk("ti816x_interrupt_macmisc0: unhandled interrupt\n"
				"\tMACINVECTOR: %#lx\n"
				"\tCMMISCINTSTAT: %#lx\n",
				macinvector,
				REG_LOAD(EMAC_CTRL_BASE + EMAC_R_CMMISCINTSTAT));
	}

	REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, MISCEOI);

	return IRQ_HANDLED;
}

static void ti816x_config(struct net_device *dev) {
	unsigned char bcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	/* check extra header size */
	assert(skb_max_extra_size() == sizeof(struct emac_desc_head));

	/* reset EMAC module */
	emac_reset();

	/* disable all the EMAC/MDIO interrupts in the control module */
	emac_ctrl_disable_irq();

	/* initialization of the EMAC control module */
	emac_ctrl_enable_pacing();

	/* load device MAC-address */
	cm_load_mac(dev);

	/* initialization of EMAC and MDIO modules */
	emac_clear_ctrl_regs();
	emac_clear_hdp();
	emac_set_stat_regs(0);
	emac_set_macaddr(&bcast_addr);
	emac_init_rx_regs();
	emac_clear_machash();
	emac_set_rxbufoff(0);
	emac_clear_and_enable_rxunicast();
	emac_enable_rxmbp();
	emac_set_macctrl(FULLDUPLEX | TXPACE);
	emac_enable_rx_and_tx_irq();
	emac_alloc_rx_queue(MODOPS_PREP_BUFF_CNT,
			netdev_priv(dev, struct ti816x_priv));
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
