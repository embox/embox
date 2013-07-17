/*
 * @file
 *
 * @date Sep 13, 2012
 * @author: Anton Bondarev
 */

#include <embox/unit.h>
#include <errno.h>
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

#include <unistd.h>


EMBOX_UNIT_INIT(greth_init);
/* control register bits */
#define GRETH_CTRL_RESET      0x00000040
#define GRETH_CTRL_FD         0x00000010
#define GRETH_CTRL_PR         0x00000020
#define GRETH_CTRL_SP         0x00000080
#define GRETH_CTRL_GB         0x00000100
#define GRETH_CTRL_PSTATIEN   0x00000400
#define GRETH_CTRL_MCEN       0x00000800
#define GRETH_CTRL_DISDUPLEX  0x00001000

/*
 * descriptor status registers bits both for rx and tx
 */
#define GRETH_BD_EN 0x0800
#define GRETH_BD_WR 0x1000
#define GRETH_BD_IE 0x2000



struct greth_regs {
	uint32_t control;
	uint32_t status;
	uint32_t mac_msb;
	uint32_t mac_lsb;
	uint32_t mdio;
	uint32_t tx_desc_p;
	uint32_t rx_desc_p;
	uint32_t edcl_ip;
	uint32_t hash_msb;
	uint32_t hash_lsb;
};
static struct greth_regs *dev_regs;

/* GRETH buffer descriptor */
struct greth_bd {
	uint32_t status;
	uint32_t address;
};

#define GRETH_RX_DESC_QUANTITY    0x8
#define GRETH_TX_DESC_QUANTITY    0x1

/* we must align buffer by 1k, for aligned each buffer item (frame if 0x600 byte)
 * we need set item site to 0x800 byte
 */
static char tx_buff[GRETH_TX_DESC_QUANTITY][0x800] __attribute__((aligned(0x400)));
static char rx_buff[GRETH_RX_DESC_QUANTITY][0x800] __attribute__((aligned(0x400)));

static struct greth_bd tx_bd_array[GRETH_TX_DESC_QUANTITY];
static struct greth_bd rx_bd_array[GRETH_RX_DESC_QUANTITY];

static void greth_bd_setup(struct greth_bd *bd) {

}

static void greth_rings_init(void) {
	int i;

	/* Initialize descriptor base address */
	REG_STORE(&dev_regs->tx_desc_p, (uint32_t)tx_bd_array);
	REG_STORE(&dev_regs->rx_desc_p, (uint32_t)rx_bd_array);

	/* initialize tx ring buffer descriptors */
	for(i = 0; i < ARRAY_SIZE(tx_bd_array); i ++) {
		tx_bd_array[i].address = (uint32_t)tx_buff[i];
		greth_bd_setup(&tx_bd_array[i]);
	}

	/* initialize rx ring buffer descriptors */
	for(i = 0; i < ARRAY_SIZE(rx_bd_array); i ++) {
		rx_bd_array[i].address = (uint32_t)rx_buff[i];
		greth_bd_setup(&tx_bd_array[i]);
	}
}

static int greth_xmit(struct net_device *dev, struct sk_buff *skb) {
	return ENOERR;
}

static int greth_set_mac_address(struct net_device *dev, const void *addr) {
	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}
	REG_STORE(&dev_regs->mac_msb, dev->dev_addr[0] << 8 | dev->dev_addr[1]);
	REG_STORE(&dev_regs->mac_lsb, dev->dev_addr[2] << 24 | dev->dev_addr[3] << 16 |
		      dev->dev_addr[4] << 8 | dev->dev_addr[5]);

	return ENOERR;
}

static int greth_reset(void) {
	/* Reset the controller. */
	REG_STORE(&dev_regs->control, GRETH_CTRL_RESET);

	/* Wait for MAC to reset itself */
	sleep(1);

	if(REG_LOAD(dev_regs) & GRETH_CTRL_RESET) {
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
	.xmit      = greth_xmit,
	.start = greth_open,
	.stop = greth_stop,
	.set_macaddr = greth_set_mac_address

};

static irq_return_t greth_irq_handler(unsigned int irq_num, void *dev_id) {
	return IRQ_HANDLED;
}

#ifdef DRIVER_AMBAPP
#include <kernel/printk.h>
static int dev_regs_init(unsigned int *irq_nr) {
	amba_dev_t amba_dev;

	assert(NULL != irq_nr);
	if (-1 == capture_amba_dev(&amba_dev, AMBAPP_VENDOR_GAISLER,
			AMBAPP_DEVICE_GAISLER_ETHMAC, false, false)) {
		printk("can't capture apb dev venID=0x%X, devID=0x%X\n",
				AMBAPP_VENDOR_GAISLER, AMBAPP_DEVICE_GAISLER_GPTIMER);
		return -ENODEV;
	}
	dev_regs = (struct greth_regs *) amba_dev.bar[0].start;
	*irq_nr = amba_dev.dev_info.irq;
	return 0;
}
#elif OPTION_DEFINED(NUMBER,gptimer_base)
static int dev_regs_init(unsigned int *irq_nr) {
	assert(NULL != irq_nr);
	dev_regs = (volatile struct gptimer_regs *) OPTION_GET(NUMBER,greth_base);
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


	nic = etherdev_alloc();
	if (nic == NULL) {
		return -ENOMEM;
	}

	dev_regs_init(&irq);

	nic->drv_ops = &greth_ops;
	nic->irq = irq;
	nic->base_addr = (uint32_t)dev_regs;

	greth_reset();

	greth_rings_init();

	/* Clear all pending interrupts except PHY irq */
	REG_STORE(&dev_regs->status, 0xFF);


	res = irq_attach(nic->irq, greth_irq_handler, 0, nic, "greth");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}
