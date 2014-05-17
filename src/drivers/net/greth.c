/**
 * @file
 *
 * @date Sep 13, 2012
 * @author: Anton Bondarev
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
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <util/binalign.h>


#include <kernel/printk.h>


EMBOX_UNIT_INIT(greth_init);

#define GRETH_DB_QUANTITY 128 /* 1kbyte / 8 (2 word) */

/* control register bits */
#define GRETH_CTRL_TX_EN    (1 << 0)
#define GRETH_CTRL_RX_EN    (1 << 1)
#define GRETH_CTRL_TX_INT   (1 << 2)
#define GRETH_CTRL_RX_INT   (1 << 3)

#define GRETH_CTRL_FD         0x00000010 /* full duplex */
#define GRETH_CTRL_PROMISC    0x00000020
#define GRETH_CTRL_RESET      0x00000040
#define GRETH_CTRL_SPEED      0x00000080
#define GRETH_CTRL_GB         0x00000100
#define GRETH_CTRL_PSTATIEN   0x00000400
#define GRETH_CTRL_MCEN       0x00000800
#define GRETH_CTRL_DISDUPLEX  0x00001000

/*
 * descriptor status registers bits both for rx and tx
 */
#define GRETH_BD_EN       0x0800
#define GRETH_BD_WR       0x1000
#define GRETH_BD_IE       0x2000

#define GRETH_BD_LEN_MASK 0x07FF


/* GRETH buffer descriptor */
struct greth_bd {
	volatile uint32_t status;
	uint32_t address;
};

struct greth_regs {
	uint32_t control;
	uint32_t status;
	uint32_t mac_msb;
	uint32_t mac_lsb;
	uint32_t mdio;
	struct greth_bd *tx_desc_p;
	struct greth_bd *rx_desc_p;
	uint32_t edcl_ip;
	uint32_t hash_msb;
	uint32_t hash_lsb;
};


struct greth_dev {
	struct greth_regs *base;

	int irq_num;
	int rx_bd;
	int rx_next;
	int tx_bd;
	int tx_next;
};

static struct greth_bd tx_bd[GRETH_DB_QUANTITY] __attribute__((aligned(0x400)));
static struct greth_bd rx_bd[GRETH_DB_QUANTITY] __attribute__((aligned(0x400)));

static struct sk_buff *rx_skb[GRETH_DB_QUANTITY];

static struct greth_dev greth_dev;

static struct greth_bd *greth_alloc_rx_bd(struct greth_dev *dev, struct sk_buff *skb) {
	struct greth_bd *bd = dev->base->rx_desc_p;

	assert(binalign_check_bound((uintptr_t)skb->mac.raw, 4));
	bd->address = (uint32_t)skb->mac.raw;
	bd->status = GRETH_BD_EN | GRETH_BD_IE;

	rx_skb[dev->rx_next] = skb;

	dev->rx_next++;
	dev->rx_next %= GRETH_DB_QUANTITY;

	return bd;
}

static struct greth_bd *greth_alloc_tx_bd(struct greth_dev *dev, struct sk_buff *skb) {
	struct greth_bd *bd = dev->base->tx_desc_p;

	assert(binalign_check_bound((uintptr_t)skb->mac.raw, 4));
	bd->address = (uint32_t)skb->mac.raw;
	bd->status = GRETH_BD_EN | skb->len;

	dev->tx_next++;
	dev->tx_next %= GRETH_DB_QUANTITY;

	return bd;
}

static void greth_rings_init(struct greth_dev *dev) {
	struct sk_buff *skb;
	struct greth_regs *regs = dev->base;

	/* Initialize descriptor base address */
	REG_STORE(&regs->tx_desc_p, (uint32_t)tx_bd);
	REG_STORE(&regs->rx_desc_p, (uint32_t)rx_bd);

	dev->rx_bd = 0;
	dev->rx_next = 0;

	dev->tx_bd = 0;
	dev->tx_next = 0;


	/* initialize rx ring buffer descriptors */
	skb = skb_alloc(ETH_FRAME_LEN);
	greth_alloc_rx_bd(dev, skb);

}

#define DEBUG 0
#if DEBUG
#include <kernel/printk.h>
/* Debugging routines */
static inline void show_packet(uint8_t *raw, int size, char *title) {
	int i;

	irq_lock();
	printk("\nPACKET(%d) %s:", size, title);
	for (i = 0; i < size; i++) {
		if (!(i % 16)) {
			printk("\n");
		}
		printk(" %02hhX", *(raw + i));
	}
	printk("\n.\n");
	irq_unlock();
}
#else
static inline void show_packet(uint8_t *raw, int size, char *title) {
}
#endif


static int greth_xmit(struct net_device *dev, struct sk_buff *skb) {
	volatile struct greth_bd *bd;

	bd = greth_alloc_tx_bd(&greth_dev, skb);
	REG_ORIN(&greth_dev.base->control, GRETH_CTRL_TX_EN);
	show_packet(skb->mac.raw, skb->len, "transmite");
	while(bd->status & GRETH_BD_EN);

	skb_free(skb);

	return ENOERR;
}

static int greth_set_mac_address(struct net_device *dev, const void *addr) {
	struct greth_regs *regs = greth_dev.base;

	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}
	REG_STORE(&regs->mac_msb, dev->dev_addr[0] << 8 | dev->dev_addr[1]);
	REG_STORE(&regs->mac_lsb, dev->dev_addr[2] << 24 |
			dev->dev_addr[3] << 16 | dev->dev_addr[4] << 8 | dev->dev_addr[5]);

	return ENOERR;
}

static int greth_reset(void) {
	struct greth_regs *regs = greth_dev.base;

	/* Reset the controller. */
	REG_STORE(&regs->control, GRETH_CTRL_RESET);

	/* Wait for MAC to reset itself */
	sleep(1);

	if(REG_LOAD(regs->control) & GRETH_CTRL_RESET) {
		return -ETIMEDOUT;
	}

	return ENOERR;
}

static int greth_open(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

	return ENOERR;
}

static int greth_stop(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

	return ENOERR;
}

static const struct net_driver greth_ops = {
	.xmit = greth_xmit,
	.start = greth_open,
	.stop = greth_stop,
	.set_macaddr = greth_set_mac_address

};


static irq_return_t greth_received(struct net_device * dev) {
	struct sk_buff *skb, *skb_new;
	unsigned int len;

	len = rx_bd[greth_dev.rx_bd].status & GRETH_BD_LEN_MASK;
	skb = rx_skb[greth_dev.rx_bd];

	skb->len = len;
	skb->dev = dev;

	greth_dev.rx_bd ++;
	greth_dev.rx_bd %= GRETH_DB_QUANTITY;
	skb_new = skb_alloc(ETH_FRAME_LEN);
	greth_alloc_rx_bd(&greth_dev, skb_new);
	REG_ORIN(&greth_dev.base->control, GRETH_CTRL_RX_INT | GRETH_CTRL_RX_EN);


	show_packet(skb->mac.raw, len, "received");

	netif_rx(skb);

	return IRQ_HANDLED;
}

static irq_return_t greth_irq_handler(unsigned int irq_num, void *dev_id) {
	return greth_received(dev_id);
}

#ifdef DRIVER_AMBAPP
#include <kernel/printk.h>
static int dev_regs_init(unsigned int *irq_nr) {
	amba_dev_t amba_dev;

	assert(NULL != irq_nr);
	if (-1 == capture_amba_dev(&amba_dev, AMBAPP_VENDOR_GAISLER,
			AMBAPP_DEVICE_GAISLER_ETHMAC, false, false)) {
		printk("can't capture apb dev venID=0x%X, devID=0x%X\n",
				AMBAPP_VENDOR_GAISLER, AMBAPP_DEVICE_GAISLER_ETHMAC);
		return -ENODEV;
	}
	greth_dev.base = (struct greth_regs *) amba_dev.bar[0].start;
	*irq_nr = amba_dev.dev_info.irq;
	return 0;
}
#elif OPTION_DEFINED(NUMBER,greth_base)
static int dev_regs_init(unsigned int *irq_nr) {
	assert(NULL != irq_nr);
	greth_dev.base = (struct greth_regs *) OPTION_GET(NUMBER,greth_base);
	*irq_nr = OPTION_GET(NUMBER,irq_num);
	return 0;
}
#else
# error "Either DRIVER_AMBAPP or gptimer_base must be defined"
#endif /* DRIVER_AMBAPP */

/*
 * initializing procedure
 */
static int greth_init(void) {
	struct net_device *nic;
	int res;
	unsigned int irq;
	char hw_addr[] = {0x0,0x0,0x7a,0xcc,0x0,0x13};

	res = dev_regs_init(&irq);
	if (res != 0) {
		return res;
	}

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &greth_ops;
	nic->irq = irq;
	nic->base_addr = (uint32_t)greth_dev.base;

	greth_reset();

	greth_rings_init(&greth_dev);

	memcpy(nic->dev_addr, hw_addr, 6);

	greth_set_mac_address(nic, nic->dev_addr);


	/* Clear all pending interrupts except PHY irq */
	REG_STORE(&greth_dev.base->status, 0xFF);


	res = irq_attach(nic->irq, greth_irq_handler, 0, nic, "greth");
	if (res < 0) {
		return res;
	}

	REG_STORE(&greth_dev.base->control, GRETH_CTRL_RX_INT | GRETH_CTRL_RX_EN);

	return inetdev_register_dev(nic);
}

