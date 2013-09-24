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
#include <drivers/amba_pnp.h>
#include <net/l0/net_entry.h>

#include <net/l2/ethernet.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

#include <kernel/printk.h>

EMBOX_UNIT_INIT(ti8178_init);

#define PREP_BUF_COUNT 5

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

static void emac_clear_hdp(void) {
	int i;

	for (i = 0; i < EMAC_CHANNEL_COUNT; ++i) {
		REG_STORE(EMAC_BASE + EMAC_TX_HDP(i), 0);
		REG_STORE(EMAC_BASE + EMAC_RX_HDP(i), 0);
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

static void emac_set_macaddr(unsigned char (*addr)[6]) {
	int i;

	for (i = 0; i < EMAC_CHANNEL_COUNT; ++i) {
		REG_STORE(EMAC_BASE + EMAC_R_MACINDEX, i & 0x1f);
		REG_STORE(EMAC_BASE + EMAC_R_MACADDRHI,
				((*addr)[2] << 24) | ((*addr)[3] << 16)
				| ((*addr)[4] << 8) | ((*addr)[5] << 0));
		REG_STORE(EMAC_BASE + EMAC_R_MACADDRLO,
				(0x1 << 20) | (0x1 << 19) | ((i & 0x7) << 16)
				| ((*addr)[0] << 8) | ((*addr)[1] << 0));
	}
}

static void emac_init_rx_regs(void) {
	int i;

	/* disable filtering */
	REG_STORE(EMAC_BASE + EMAC_R_RXFILTERLOWTHRESH, 0);

	for (i = 0; i < EMAC_CHANNEL_COUNT; ++i) {
		REG_STORE(EMAC_BASE + EMAC_R_RXFLOWTHRESH, 0);

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

#define TXPACE (0x1 << 6)
#define GMIIEN (0x1 << 5)
static void emac_set_macctrl(unsigned long v) {
	REG_ORIN(EMAC_BASE + EMAC_R_MACCONTROL, v);
}

#define HOSTMASK (0x1 << 1)
#define STATMASK (0x1 << 0)
static void emac_enable_rx_and_tx_irq(void) {
	/* mask unused channel */
	//REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKCLEAR, 0xf);
	//REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKCLEAR, 0xf);

	/* allow all rx and tx interrupts */
	REG_STORE(EMAC_BASE + EMAC_R_TXINTMASKSET, 0xf);
	REG_STORE(EMAC_BASE + EMAC_R_RXINTMASKSET, 0xf);

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
		desc = (struct emac_desc *)skb_data;
		if (head == NULL) head = desc;
		if (prev != NULL) prev->next = desc;
		desc->next = 0;
		desc->data = desc + 1;
		desc->data_off = 0;
		desc->data_len = skb_max_size() - sizeof *desc;
		desc->flags = EMAC_DESC_F_SOP | EMAC_DESC_F_EOP | EMAC_DESC_F_OWNER;
		desc->len = desc->data_len;
		prev = desc;
	}

	REG_STORE(EMAC_BASE + EMAC_R_RXHDP(0), head);
}

#define RXEN (0x1 << 0)
#define TXEN (0x1 << 0)
static void emac_enable_rx_and_tx_dma(void) {
	REG_STORE(EMAC_BASE + EMAC_R_RXCONTROL, RXEN);
	REG_STORE(EMAC_BASE + EMAC_R_TXCONTROL, TXEN);
}

static int ti8178_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

static int ti8178_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static int ti8178_open(struct net_device *dev) {
	emac_ctrl_enable_irq();
	return 0;
}

static int ti8178_stop(struct net_device *dev) {
	emac_ctrl_disable_irq();
	return 0;
}

static const struct net_driver ti8178_ops = {
	.xmit = ti8178_xmit,
	.start = ti8178_open,
	.stop = ti8178_stop,
	.set_macaddr = ti8178_set_macaddr

};

static irq_return_t ti8178_interrupt(unsigned int irq_num, void *dev_id) {
}

#include <kernel/printk.h>
static void ti8178_config(struct netdevice *dev) {
	unsigned char addr[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x02 };

	printk("CPGMACIDVER %#x\n", REG_LOAD(EMAC_BASE + EMAC_R_CPGMACIDVER));

	/* TODO enable EMAC/MDIO peripheral */

	/* TODO configuration of the interrupt to the CPU */

	/* disable all the EMAC/MDIO interrupts in the control module */
	emac_ctrl_disable_irq();

	/* initialization of the EMAC control module */
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMRXINTMAX, 0x4);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMTXINTMAX, 0x4);
	REG_STORE(EMAC_CTRL_BASE + EMAC_R_CMINTCTRL, 0x30000 | 0x258);

	/* initialization of EMAC and MDIO modules */
	emac_clear_hdp();
	emac_clear_ctrl_regs();
	emac_clear_stat_regs();
	emac_set_macaddr(&addr);
	emac_init_rx_regs();
	emac_clear_machash();
	emac_set_rxbufoff(0);
	emac_clear_and_enable_rxunicast();
	emac_enable_rxmbp();
	emac_set_macctrl(TXPACE);
	emac_enable_rx_and_tx_irq();
	emac_prep_rx_queue();
	emac_enable_rx_and_tx_dma();
	emac_set_macctrl(GMIIEN);

	/* enable all the EMAC/MDIO interrupts in the control module */
	emac_ctrl_enable_irq();
}

static int ti8178_init(void) {
	struct net_device *nic;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &ti8178_ops;
	nic->irq = 0;
	nic->base_addr = EMAC_BASE_ADDR;

	ti8178_config(nic);

	res = irq_attach(nic->irq, ti8178_interrupt, 0, nic, "ti8178");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}

