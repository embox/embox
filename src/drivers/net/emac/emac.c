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

#include <hal/cache.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <hal/reg.h>

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

#include <mem/vmem.h>
#include <hal/mmu.h>
#include <drivers/common/memory.h>

#include <mem/misc/pool.h>

#include "emac.h"
#include "emac_desc.h"

#include <framework/mod/options.h>

EMBOX_UNIT_INIT(ti816x_init);

#define MODOPS_PREP_BUFF_CNT	OPTION_GET(NUMBER, prep_buff_cnt)
#define RX_RING_SZ              MODOPS_PREP_BUFF_CNT

/**
 * EMAC0/MDIO Base Address
 */
#define EMAC_BASE	OPTION_GET(NUMBER, emac_base)
#define EMAC_CTRL_BASE	OPTION_GET(NUMBER, emac_ctrl_base)

/**
 * CPGMAC0 Interrupts
 */
#define MACRXTHR0 (OPTION_GET(NUMBER, irq_base) + 0) /* CPGMAC0 Receive threshold interrupt */
#define MACRXINT0 (OPTION_GET(NUMBER, irq_base) + 1) /* CPGMAC0 Receive pending interrupt */
#define MACTXINT0 (OPTION_GET(NUMBER, irq_base) + 2) /* CPGMAC0 Transmit pending interrupt */
#define MACMISC0  (OPTION_GET(NUMBER, irq_base) + 3) /* CPGMAC0 Stat, Host, MDIO LINKINT or MDIO USERINT */

#define DEFAULT_CHANNEL 0
#define DEFAULT_MASK    ((uint8_t)(1 << DEFAULT_CHANNEL))

#define RX_BUFF_LEN       0x800
#define RX_FRAME_MAX_LEN  \
	min(min(RX_BUFF_LEN, skb_max_size()), (ETH_FRAME_LEN + ETH_FCS_LEN))

struct emac_desc_head {
	uint8_t padding[64];
	struct emac_desc desc;
	uint8_t padding2[64];
	void *skb;
} __attribute__ ((aligned (0x4)));

struct ti816x_priv {
	volatile struct emac_desc *tx_cur_head;
	volatile struct emac_desc *tx_wait_head;
	volatile struct emac_desc *tx_wait_tail;
};

static struct emac_desc_head *head_from_desc(struct emac_desc *desc) {
	return (struct emac_desc_head *) (((char*)desc - 64));
}

static uint8_t emac_rx_data_buff[RX_BUFF_LEN][RX_RING_SZ] __attribute__ ((aligned (0x4)));
static struct emac_desc_head emac_rx_list[RX_RING_SZ];
POOL_DEF(emac_tx_desc_pool, struct emac_desc_head, MODOPS_PREP_BUFF_CNT + 0x1);

static struct emac_desc_head *emac_hdesc_tx_alloc(void) {
	ipl_t sp;
	struct emac_desc_head *res;

	sp = ipl_save();
	{
		res = pool_alloc(&emac_tx_desc_pool);
	}
	ipl_restore(sp);

	return res;
}

static void emac_hdesc_tx_free(struct emac_desc_head *obj) {
	ipl_t sp;
	assert(obj);

	sp = ipl_save();
	{
		pool_free(&emac_tx_desc_pool, obj);
	}
	ipl_restore(sp);
}

static void emac_desc_set_next(struct emac_desc *desc, void *next) {
	desc->next = (uintptr_t) next;
	dcache_flush(desc, sizeof(struct emac_desc));
}

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

static inline void emac_ctrl_enable_pacing(void) {
	/* enable rx/tx pacing; set pacing period */
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMINTCTRL,
			EMAC_CMINTCTRL_C0_RX | EMAC_CMINTCTRL_C0_TX |
			EMAC_CMINTCTRL_INTPRESCALE(125 * 4));

	/* set maximum number of rx/tx interrupts */
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXINTMAX, EMAC_CMRXINTMAX_RXIMAX(4));
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMTXINTMAX, EMAC_CMTXINTMAX_TXIMAX(4));
}

static void emac_eoi(int flag) {
	REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, flag);

#if EMAC_VERSION == 1
	if (flag == EMAC_MACEOIVEC_TXEOI) {
		REG_STORE(LVL_INTR_CLEAR, 1 << 3);
	} else if (flag == EMAC_MACEOIVEC_RXTHRESHEOI) {
		REG_STORE(LVL_INTR_CLEAR, 1 << 2);
	} else if (flag == EMAC_MACEOIVEC_RXEOI) {
		REG_STORE(LVL_INTR_CLEAR, 1 << 1);
	} else if (flag == EMAC_MACEOIVEC_MISCEOI) {
		REG_STORE(LVL_INTR_CLEAR, 1 << 0);
	}
#endif
}

static void cm_load_mac(struct net_device *dev) {
	unsigned long mac_hi, mac_lo;

#if EMAC_VERSION == 0
	mac_hi = REG_LOAD(CM_BASE + CM_R_MACID0_HI);
	mac_lo = REG_LOAD(CM_BASE + CM_R_MACID0_LO);
#else
	mac_hi = 0xdeadca;
	mac_lo = 0xfebabe;
#endif
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

	mac_hi = ((*addr)[2] << 24) | ((*addr)[3] << 16)
			| ((*addr)[4] << 8) | ((*addr)[5] << 0);
	mac_lo = ((*addr)[0] << 8) | ((*addr)[1] << 0);

	for (i = 0; i < EMAC_CHANNEL_COUNT; ++i) {
		REG_STORE(EMAC_BASE + EMAC_R_MACINDEX, MACINDEX(i));
		REG_STORE(EMAC_BASE + EMAC_R_MACADDRHI, mac_hi);
		REG_STORE(EMAC_BASE + EMAC_R_MACADDRLO,
				mac_lo | VALID | MATCHFILT | CHANNEL(i));
	}

	log_debug("set macaddr %06x %06x", mac_hi, mac_lo);

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

static void emac_enable_rxmbp(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXMBPENABLE, RXMBP_INIT);
}

#define GIG_FORCE (0x1 << 17)
#define GIG (0x1 << 7)
#define TXPACE (0x1 << 6)
#define GMIIEN (0x1 << 5)
#define FULLDUPLEX (0x1 << 0)
void emac_set_macctrl(unsigned long v) {
	REG_STORE(EMAC_BASE + EMAC_R_MACCONTROL, v);
}

static void emac_enable_rx_and_tx_irq(void) {
	/* mask unused channel */
	REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKCLEAR, ~DEFAULT_MASK);
	REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKCLEAR, ~DEFAULT_MASK);

	/* allow all rx and tx interrupts */
	REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKSET, DEFAULT_MASK);
	REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKSET, DEFAULT_MASK);

	/* allow host error and statistic interrupt */
	REG_STORE(EMAC_BASE + EMAC_R_MACINTMASKSET, EMAC_MACINT_HOSTMASK | EMAC_MACINT_STATMASK);
}

static void emac_enable_rx_and_tx_dma(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXCONTROL, EMAC_RXCONTROL_RXEN);
	REG_STORE(EMAC_BASE + EMAC_R_TXCONTROL, EMAC_TXCONTROL_TXEN);
}

static void emac_desc_build(struct emac_desc *desc, uint8_t *data,
		size_t data_len, size_t packet_len, int flags) {
	assert(desc != NULL);
	assert(binalign_check_bound((uintptr_t)desc, 4));
	assert(data_len != 0);
	assert(flags & EMAC_DESC_F_OWNER);

	desc->next = 0;
	desc->data = (uintptr_t)data;
	desc->data_len = data_len;
	desc->packet_len = packet_len;
	desc->data_off = 0;
	desc->flags = flags;

	dcache_flush(desc, sizeof(struct emac_desc));
}

static void emac_queue_activate(struct emac_desc *desc, uint32_t reg_hdp) {
	assert(desc != NULL);
	REG_STORE(EMAC_BASE + reg_hdp, (uintptr_t)desc);
	dcache_flush((void*)(EMAC_BASE + reg_hdp), sizeof (uintptr_t));
}

static void emac_alloc_rx_queue(struct ti816x_priv *dev_priv) {
	int i;
	assert(dev_priv);

	for (i = 0; i < RX_RING_SZ; i++) {
		struct emac_desc *desc;
		struct emac_desc *next;
		int idx_next = (i + 1) % RX_RING_SZ;

		desc = &emac_rx_list[i].desc;
		next = &emac_rx_list[idx_next].desc;

		emac_rx_list[i].skb = emac_rx_data_buff[i];
		emac_desc_build(desc, emac_rx_data_buff[i],
				RX_BUFF_LEN, 0, EMAC_DESC_F_OWNER);
		emac_desc_set_next(desc, next);
	}

	emac_queue_activate(&emac_rx_list[0].desc, EMAC_R_RXHDP(DEFAULT_CHANNEL));
}

static int ti816x_xmit(struct net_device *dev, struct sk_buff *skb) {
	size_t data_len;
	struct emac_desc_head *hdesc;
	struct emac_desc *desc;
	struct ti816x_priv *dev_priv;
	ipl_t ipl;

	hdesc = emac_hdesc_tx_alloc();
	if (!hdesc) {
		skb_free(skb);
		return 0;
	}
	hdesc->skb = skb;
	desc = &hdesc->desc;

	data_len = max(skb->len, ETH_ZLEN);
	dcache_flush(skb_data_cast_in(skb->data), data_len);

	emac_desc_build(desc, skb_data_cast_in(skb->data), data_len, data_len,
			EMAC_DESC_F_SOP | EMAC_DESC_F_EOP | EMAC_DESC_F_OWNER);

	dev_priv = netdev_priv(dev);
	assert(dev_priv != NULL);

	ipl = ipl_save();
	{
		if (dev_priv->tx_cur_head) {
			/* Add packet to the waiting queue */
			if (dev_priv->tx_wait_head) {
				emac_desc_set_next((struct emac_desc *)dev_priv->tx_wait_tail, desc);
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

static irq_return_t ti816x_interrupt_macrxthr0(unsigned int irq_num,
		void *dev_id) {
	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE
				+ EMAC_R_CMRXTHRESHINTSTAT));

	log_debug("ti816x_interrupt_macrxthr0: unhandled interrupt");

	emac_eoi(EMAC_MACEOIVEC_RXTHRESHEOI);

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
	struct emac_desc *desc;
	struct emac_desc *next;
	struct emac_desc_head *hdesc;
	struct sk_buff *skb;

	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE
				+ EMAC_R_CMRXINTSTAT));

	dev_priv = netdev_priv(dev_id);
	assert(dev_priv != NULL);

	desc = (struct emac_desc *)REG_LOAD(EMAC_BASE + EMAC_R_RXCP(DEFAULT_CHANNEL));
	do {
		dcache_inval(desc, sizeof (struct emac_desc));

		if (desc->flags & EMAC_DESC_F_EOQ) {
			break;
		}
		if (desc->flags & EMAC_DESC_F_PASSCRC) {
			desc->data_len -= 4;
		}

		assert(!(desc->flags & EMAC_DESC_F_TDOWNCMPLT));
		assert(!(desc->flags & EMAC_DESC_F_EOQ));

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

		next = (void*) desc->next;

		hdesc = head_from_desc(desc);
		emac_desc_build(desc, hdesc->skb, RX_FRAME_MAX_LEN, 0, EMAC_DESC_F_OWNER);
		emac_desc_set_next(desc, next);

		REG_STORE(EMAC_BASE + EMAC_R_RXCP(DEFAULT_CHANNEL), (uintptr_t)desc);

		desc = next;
	} while (desc !=
			(struct emac_desc *)REG_LOAD(EMAC_BASE + EMAC_R_RXCP(DEFAULT_CHANNEL)));

	emac_eoi(EMAC_MACEOIVEC_RXEOI);

	return IRQ_HANDLED;
}


#define CHECK_TXERR(x) \
	((x) & (EMAC_DESC_F_OWNER | EMAC_DESC_F_TDOWNCMPLT))

static irq_return_t ti816x_interrupt_mactxint0(unsigned int irq_num,
		void *dev_id) {
	struct ti816x_priv *dev_priv;
	int eoq;
	ipl_t sp;
	struct emac_desc *desc;
	struct emac_desc_head *hdesc;

	assert(DEFAULT_MASK == REG_LOAD(EMAC_CTRL_BASE + EMAC_R_CMTXINTSTAT));

	dev_priv = netdev_priv(dev_id);
	assert(dev_priv != NULL);

	desc = 	(struct emac_desc *)REG_LOAD(EMAC_BASE + EMAC_R_TXCP(DEFAULT_CHANNEL));
	dcache_inval(desc, sizeof *desc);
	assert(!CHECK_TXERR(desc->flags));

	eoq = desc->flags & EMAC_DESC_F_EOQ;

	REG_STORE(EMAC_BASE + EMAC_R_TXCP(DEFAULT_CHANNEL), (uintptr_t)desc);

	emac_eoi(EMAC_MACEOIVEC_TXEOI);

	if (!eoq) {
		assert(desc->next);
		goto out;
	}
	assert(!desc->next);
	sp = ipl_save();
	{
		dev_priv->tx_cur_head = dev_priv->tx_wait_head;
		if (dev_priv->tx_wait_head) {
			dev_priv->tx_wait_head = NULL;
			dev_priv->tx_wait_tail = NULL;
			emac_queue_activate((struct emac_desc *)dev_priv->tx_cur_head,
					EMAC_R_TXHDP(DEFAULT_CHANNEL));
		}
	}
	ipl_restore(sp);

out:
	hdesc = head_from_desc(desc);
	skb_free(hdesc->skb);
	emac_hdesc_tx_free(hdesc);

	return IRQ_HANDLED;
}

static irq_return_t ti816x_interrupt_macmisc0(unsigned int irq_num,
		void *dev_id) {
	unsigned long macinvector;

	macinvector = REG_LOAD(EMAC_BASE + EMAC_R_MACINVECTOR);

	if (macinvector & EMAC_MACINV_STATPEND) {
		emac_set_stat_regs(0xffffffff);
		macinvector &= ~EMAC_MACINV_STATPEND;
	}
	if (macinvector & EMAC_MACINV_HOSTPEND) {
		unsigned long macstatus;

		macstatus = REG_LOAD(EMAC_BASE + EMAC_R_MACSTATUS);
		log_raw(LOG_ERR,
		    "\tMACSTATUS: %#lx\n"
		    "\t\tidle %lx\n"
		    "\t\ttxerrcode %lx; txerrch %lx\n"
		    "\t\trxerrcode %lx; rxerrch %lx\n"
		    "\t\trgmiigig %lx; rgmiifullduplex %lx\n"
		    "\t\trxqosact %lx; rxflowact %lx; txflowact %lx\n",
		    macstatus, EMAC_MACSTAT_IDLE(macstatus),
		    EMAC_MACSTAT_TXERRCODE(macstatus), EMAC_MACSTAT_TXERRCH(macstatus),
		    EMAC_MACSTAT_RXERRCODE(macstatus), EMAC_MACSTAT_RXERRCH(macstatus),
		    EMAC_MACSTAT_RGMIIGIG(macstatus),
		    EMAC_MACSTAT_RGMIIFULLDUPLEX(macstatus),
		    EMAC_MACSTAT_RXQOSACT(macstatus), EMAC_MACSTAT_RXFLOWACT(macstatus),
		    EMAC_MACSTAT_TXFLOWACT(macstatus));

		emac_reset();
		ti816x_config((struct net_device *)dev_id);

		macinvector &= ~EMAC_MACINV_HOSTPEND;
	}
	if (macinvector & (EMAC_MACINV_RXPEND | EMAC_MACINV_TXPEND)) {
		/* umm.. ok */
		macinvector &= ~(EMAC_MACINV_RXPEND | EMAC_MACINV_TXPEND);
	}
	if (macinvector) {
		log_raw(LOG_DEBUG,
		    "ti816x_interrupt_macmisc0: unhandled interrupt\n"
		    "\tMACINVECTOR: %#lx\n"
		    "\tCMMISCINTSTAT: %#lx\n",
		    macinvector, REG_LOAD(EMAC_CTRL_BASE + EMAC_R_CMMISCINTSTAT));
	}

	emac_eoi(EMAC_MACEOIVEC_MISCEOI);

	return IRQ_HANDLED;
}

static void emac_set_max_frame_len(int m) {
	REG_STORE(EMAC_BASE + EMAC_R_RXMAXLEN, m);
}

static void ti816x_config(struct net_device *dev) {
	struct ti816x_priv *ti816x_priv;
	unsigned char bcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	ti816x_priv = netdev_priv(dev);
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
	emac_set_max_frame_len(RX_FRAME_MAX_LEN);
	emac_clear_and_enable_rxunicast();
	emac_enable_rxmbp();
	emac_mdio_config();
	emac_alloc_rx_queue(ti816x_priv);
	emac_enable_rx_and_tx_irq();
	emac_enable_rx_and_tx_dma();

	REG_ORIN(EMAC_BASE + EMAC_R_MACCONTROL, 0x20);

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
	nic->irq = MACRXTHR0;
	nic->base_addr = EMAC_BASE;

	nic_priv = netdev_priv(nic);
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

PERIPH_MEMORY_DEFINE(emac_region, EMAC_BASE, 0x800);

PERIPH_MEMORY_DEFINE(emac_ctrl_region, EMAC_CTRL_BASE, 0x800);

#if EMAC_VERSION == 1
/* Neccessary to clear interrupts */
PERIPH_MEMORY_DEFINE(emac_cm_region, LVL_INTR_CLEAR, 4);

#endif
