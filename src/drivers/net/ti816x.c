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
#include <drivers/ethernet/ti816x.h>
#include <net/l0/net_entry.h>

#include <net/l2/ethernet.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <util/binalign.h>

#include <kernel/printk.h>

EMBOX_UNIT_INIT(ti816x_init);

#define PREP_BUF_COUNT 1

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

static void cm_load_mac(struct net_device *dev) {
	unsigned long mac_hi, mac_lo;

	mac_hi = REG_LOAD(CM_BASE + CM_R_MACID0_HI);
	mac_lo = REG_LOAD(CM_BASE + CM_R_MACID0_LO);

	dev->dev_addr[0] = mac_hi & 0xff;
	dev->dev_addr[1] = (mac_hi >> 8) & 0xff;
	dev->dev_addr[2] = (mac_hi >> 16) & 0xff;
	dev->dev_addr[3] = mac_hi >> 24;
	dev->dev_addr[4] = mac_lo & 0xff;
	dev->dev_addr[5] = mac_lo >> 8;
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

static void emac_clear_stat_regs(void) {
	int i;

	for (i = 0; i < 36; ++i) {
		REG_STORE(EMAC_BASE + 0x200 + 0x4 * i, 0); /* FIXME */
	}
}

#define MACINDEX(x) ((x & 0x1f) << 0)
#define VALID (1 << 20)
#define MATCHFILT (1 << 19)
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

		REG_STORE(EMAC_BASE + EMAC_R_RXFREEBUFFER(i), PREP_BUF_COUNT);
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

#define RXNOCHAIN (0x1 << 28)
//#define RXCMFEN (0x1 << 24)
#define RXCSFEN (0x1 << 23) /* short msg */
#define RXCAFEN (0x1 << 21) /* promiscuous */
#define RXBROADEN (0x1 << 13) /* broadcast */
#define RXMULTEN (0x1 << 5) /* multicast */
static void emac_enable_rxmbp(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXMBPENABLE,
			RXNOCHAIN | RXCSFEN | RXCAFEN | RXBROADEN | RXMULTEN);
}

//#define TXPACE (0x1 << 6)
#define GMIIEN (0x1 << 5)
static void emac_set_macctrl(unsigned long v) {
	REG_ORIN(EMAC_BASE + EMAC_R_MACCONTROL, v);
}

#define HOSTMASK (0x1 << 1)
#define STATMASK (0x1 << 0)
static void emac_enable_rx_and_tx_irq(void) {
	/* mask unused channel */
	REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKCLEAR, 0xe);
	REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKCLEAR, 0xe);

	/* allow all rx and tx interrupts */
	REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKSET, 0x1);
	REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKSET, 0x1);

	/* allow host error and statistic interrupt */
	REG_STORE(EMAC_BASE + EMAC_R_MACINTMASKSET, HOSTMASK | STATMASK);
}

static void emac_prep_rx_queue(void) {
	int i;
	struct sk_buff_data *skb_data;
	struct emac_desc *desc, *head, *prev;

	head = prev = NULL;

	for (i = 0; i < PREP_BUF_COUNT; ++i) {
		skb_data = skb_data_alloc();
		assert(skb_data != NULL);
		desc = (struct emac_desc *)skb_data_get_extra_hdr(skb_data);
		assert(binalign_check_bound((uintptr_t)desc, 4));
		if (head == NULL) head = desc;
		if (prev != NULL) prev->next = (uintptr_t)desc;
		desc->next = 0;
		assert(binalign_check_bound((uintptr_t)desc->next, 4));
		desc->data = (uintptr_t)skb_data_get_data(skb_data);
		desc->data_len = skb_max_size() - sizeof *desc;
		desc->data_off = 0;
		desc->len = 0;
		desc->flags = EMAC_DESC_F_OWNER;
		prev = desc;
	}

	REG_STORE(EMAC_BASE + EMAC_R_RXHDP(i), (uintptr_t)head);
}

#define RXEN (0x1 << 0)
#define TXEN (0x1 << 0)
static void emac_enable_rx_and_tx_dma(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXCONTROL, RXEN);
	REG_STORE(EMAC_BASE + EMAC_R_TXCONTROL, TXEN);
}

static int ti816x_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct sk_buff_data *skb_data;
	struct emac_desc *desc;

	skb_data = skb_data_clone(skb->data);

	assert(skb_data != NULL);

	desc = (struct emac_desc *)skb_data_get_extra_hdr(skb_data);
	assert(binalign_check_bound((uintptr_t)desc, 32));

	desc->next = 0;
	assert(binalign_check_bound(desc->next, 32));
	desc->data = (uintptr_t)skb->mac.raw;
	desc->data_len = desc->len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
	desc->data_off = 0;
	desc->flags = EMAC_DESC_F_SOP | EMAC_DESC_F_EOP | EMAC_DESC_F_OWNER;

	skb_free(skb);

	REG_STORE(EMAC_BASE + EMAC_R_TXHDP(0), (uintptr_t)desc);

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

#include <kernel/printk.h>
#define HOSTPEND (1 << 26)
#define RXTHRESHEOI 0x0
#define RXEOI       0x1
#define TXEOI       0x2
#define MISCEOI     0x3
static irq_return_t ti816x_interrupt(unsigned int irq_num, void *dev_id) {
	unsigned long reg;
	struct sk_buff *skb;
	struct sk_buff_data *new_data;
	struct emac_desc *desc;

	//printk("!%u reg %lx\n", irq_num, REG_LOAD(EMAC_BASE + EMAC_R_MACINVECTOR));

	switch (irq_num) {
	case MACRXINT0:
		reg = REG_LOAD(EMAC_BASE + EMAC_R_RXCP(0));
		desc = (struct emac_desc *)reg;
		skb = skb_wrap(desc->len, sizeof *desc, (struct sk_buff_data *)desc);
		if (skb == NULL) {
			printk("ti816x_interrupt: error: skb_wrap return NULL\n");
			break;
		}
		skb->dev = dev_id;
		netif_rx(skb);
		new_data = skb_data_alloc();
		if (new_data != NULL) {
			desc = (struct emac_desc *)skb_data_get_extra_hdr(new_data);
			desc->next = 0;
			assert(binalign_check_bound((uintptr_t)desc->next, 4));
			desc->data = (uintptr_t)skb_data_get_data(new_data);
			desc->data_len = skb_max_size() - sizeof *desc;
			desc->data_off = 0;
			desc->len = 0;
			desc->flags = EMAC_DESC_F_OWNER;
			REG_STORE(EMAC_BASE + EMAC_R_RXHDP(0), (uintptr_t)desc);
		}
		else {
			printk("ti816x_interrupt: error: skb_data_alloc return NULL\n");
		}
		REG_STORE(EMAC_BASE + EMAC_R_RXCP(0), reg);
		REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, RXEOI);
		break;
	case MACTXINT0:
		reg = REG_LOAD(EMAC_BASE + EMAC_R_TXCP(0));
		skb_data_free((struct sk_buff_data *)reg);
		//printk("tx %lx\n", reg);
		REG_STORE(EMAC_BASE + EMAC_R_TXCP(0), reg);
		REG_STORE(EMAC_BASE + EMAC_R_TXHDP(0), 0);
		REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, TXEOI);
		break;
	case MACMISC0:
		printk("cmmiscintstat %lx\n", REG_LOAD(EMAC_CTRL_BASE + EMAC_R_CMMISCINTSTAT));
		if (REG_LOAD(EMAC_BASE + EMAC_R_MACINVECTOR) & HOSTPEND) {
			printk("resetting\n");
			REG_STORE(EMAC_BASE + EMAC_R_SOFTRESET, 1);
			while (REG_LOAD(EMAC_BASE + EMAC_R_SOFTRESET) & 1);
		}
		REG_STORE(EMAC_BASE + EMAC_R_MACEOIVECTOR, MISCEOI);
		break;
	}

	return IRQ_HANDLED;
}

static void ti816x_config(struct net_device *dev) {
	unsigned char bcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	//printk("CPGMACIDVER %#lx\n", REG_LOAD(EMAC_BASE + EMAC_R_CPGMACIDVER));
	/* check extra header size */
	assert(skb_max_extra_hdr_size() == sizeof(struct emac_desc));

	/* reset EMAC module */
	REG_STORE(EMAC_BASE + EMAC_R_SOFTRESET, 1);
	while (REG_LOAD(EMAC_BASE + EMAC_R_SOFTRESET) & 1);

	/* TODO enable EMAC/MDIO peripheral */

	/* TODO configuration of the interrupt to the CPU */

	/* disable all the EMAC/MDIO interrupts in the control module */
	emac_ctrl_disable_irq();

#if 0
	/* initialization of the EMAC control module */
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXINTMAX, 0x4);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMTXINTMAX, 0x4);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMINTCTRL, 0x30000 | 0x258);
#endif

	/* load device MAC-address */
	cm_load_mac(dev);

	/* initialization of EMAC and MDIO modules */
	emac_clear_ctrl_regs();
	emac_clear_hdp();
	emac_clear_stat_regs();
	emac_set_macaddr(&bcast_addr);
	emac_init_rx_regs();
	emac_clear_machash();
	emac_set_rxbufoff(0);
	emac_clear_and_enable_rxunicast();
	emac_enable_rxmbp();
	//emac_set_macctrl(TXPACE);
	emac_enable_rx_and_tx_irq();
	emac_prep_rx_queue();
	emac_enable_rx_and_tx_dma();
	emac_set_macctrl(GMIIEN);

	/* enable all the EMAC/MDIO interrupts in the control module */
	//emac_ctrl_enable_irq();
}

static int ti816x_init(void) {
	int ret;
	struct net_device *nic;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &ti816x_ops;
	nic->irq = 0;
	nic->base_addr = EMAC_BASE_ADDR;

	ti816x_config(nic);

	ret = irq_attach(MACRXTHR0, ti816x_interrupt, 0, nic, "ti816x");
	if (ret < 0) {
		return ret;
	}
	ret = irq_attach(MACRXINT0, ti816x_interrupt, 0, nic, "ti816x");
	if (ret < 0) {
		return ret;
	}
	ret = irq_attach(MACTXINT0, ti816x_interrupt, 0, nic, "ti816x");
	if (ret < 0) {
		return ret;
	}
	ret = irq_attach(MACMISC0, ti816x_interrupt, 0, nic, "ti816x");
	if (ret < 0) {
		return ret;
	}

	return inetdev_register_dev(nic);
}
