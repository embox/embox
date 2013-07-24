/**
 * @file
 * @brief Intel e1000 gigabyte NIC series driver
 *
 * @date 01.10.2012
 * @author Anton Kozlov
 */
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <arpa/inet.h>

#include <asm/io.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/netfilter.h>

#include <net/l0/net_entry.h>

#include <hal/reg.h>

#include <drivers/ethernet/e1000.h>

#include <mem/misc/pool.h>

#include <prom/prom_printf.h>

#include <embox/unit.h>

PCI_DRIVER("e1000", e1000_init, PCI_VENDOR_ID_INTEL, PCI_DEV_ID_INTEL_82540EM);
PCI_DRIVER("e1000", e1000_init, PCI_VENDOR_ID_INTEL, PCI_DEV_ID_INTEL_82567V3);

#define MDELAY 1000

/** Number of receive descriptors per card. */
#define E1000_RXDESC_NR 16

/** Number of transmit descriptors per card. */
#define E1000_TXDESC_NR 16

/** Size of each I/O buffer per descriptor. */
#define E1000_IOBUF_SIZE 2048

struct e1000_dev {
	struct net_device dev;
	struct sk_buff_head txing_queue;
	char link_status;
};

static char rx_buf[E1000_RXDESC_NR][E1000_IOBUF_SIZE] ;
/*static char tx_buf[E1000_TXDESC_NR][E1000_IOBUF_SIZE]__attribute__((aligned(16)));*/

struct e1000_rx_desc {
	uint32_t buffer_address;
	uint32_t buffer_address_h;
	uint16_t length;
	uint16_t check_sum;
	uint8_t  status;
	uint8_t  error;
	uint16_t reserved;
};

struct e1000_tx_desc {
	uint32_t buffer_address;
	uint32_t buffer_address_h; /* not used */
	uint16_t length;
	uint8_t checksum_offload;
	uint8_t cmd;
	uint8_t status; /* + reserved, not used */
	uint8_t checksum_start;
	uint16_t special;
};

static struct e1000_rx_desc rx_descs[E1000_RXDESC_NR] __attribute__((aligned(16)));
static struct e1000_tx_desc tx_descs[E1000_TXDESC_NR] __attribute__((aligned(16)));

static void mdelay(int value) {
	volatile int delay = value;

	while (delay --);

}

static volatile uint32_t *e1000_reg(struct e1000_dev *dev, int offset) {
	return (volatile uint32_t *) (dev->dev.base_addr + offset);
}

static int e1000_xmit(struct net_device *_dev) {
	struct e1000_dev *dev = (struct e1000_dev *) _dev;
	uint16_t head = REG_LOAD(e1000_reg(dev, E1000_REG_TDH));
	uint16_t tail  = REG_LOAD(e1000_reg(dev, E1000_REG_TDT));
	struct sk_buff *skb;

	if ((tail + 1 % E1000_TXDESC_NR) == head) {
		return ENOERR;
	}

	skb = skb_queue_pop(&dev->dev.tx_dev_queue);

	if (skb == NULL) {
		return 0;
	}

	tx_descs[tail].buffer_address = (uint32_t) skb->mac.raw;
	tx_descs[tail].status = 0;
	tx_descs[tail].cmd = E1000_TX_CMD_EOP |
			        E1000_TX_CMD_FCS |
				E1000_TX_CMD_RS;
	tx_descs[tail].length  = skb->len;

	++tail;
	tail %= E1000_TXDESC_NR;

	REG_STORE(e1000_reg(dev, E1000_REG_TDT), tail);

	skb_queue_push(&dev->txing_queue, skb);

	return 0;
}

static int xmit(struct net_device *dev, struct sk_buff *skb) {
	/*prom_printf("e1000: xmit 0x%x\n", (unsigned int) skb);*/
	skb_queue_push((struct sk_buff_head *) &dev->tx_dev_queue, skb);

	e1000_xmit(dev);

	return ENOERR;
}

static void txed_skb_clean(struct net_device *_dev) {
	struct e1000_dev *dev = (struct e1000_dev *) _dev;
	struct sk_buff *skb;

	skb = skb_queue_pop(&dev->txing_queue);

	if (skb) {
		skb_free(skb);
	}
}

static void e1000_rx(struct net_device *_dev) {
	struct e1000_dev *dev = (struct e1000_dev *) _dev;
	/*net_device_stats_t stat = get_eth_stat(dev);*/
	uint16_t head = REG_LOAD(e1000_reg(dev, E1000_REG_RDH));
	uint16_t tail = REG_LOAD(e1000_reg(dev, E1000_REG_RDT));
	uint16_t cur  = (1 + tail) % E1000_RXDESC_NR;

	struct sk_buff *skb;

	while (cur != head) {
		int len = rx_descs[cur].length - 4; /* checksum */
		if (!(rx_descs[cur].status)) {
			break;
		}

		if (0 != nf_test_raw(NF_CHAIN_INPUT, NF_TARGET_ACCEPT, (void *) rx_descs[cur].buffer_address,
					ETH_ALEN + (void *) rx_descs[cur].buffer_address, ETH_ALEN)) {
			goto drop_pack;
		}

		if ((skb = skb_alloc(len))) {
			memcpy(skb->mac.raw, (void *) rx_descs[cur].buffer_address, len);
			skb->dev = (struct net_device *) dev;
			/*stat->rx_packets++;*/
			/*stat->rx_bytes += skb->len;*/

#if 0
			{
				unsigned char *p = skb->mac.raw;
				int cnt = len > 64 ? 64 : len;
				const unsigned char pat[] = { 0xaa, 0xbb, 0xcc, 0xdd};
				if (!memcmp(p, pat, 4)) {

					prom_printf("rx:\n");
					while (cnt) {
						int tcnt = 16;
						while (--tcnt && --cnt) {
							prom_printf("%02x ", *p++);
						}
						prom_printf("\n");
					}
				}
			}

#endif
			netif_rx(skb);
		} else {
			/*stat->rx_dropped++;*/
		}

drop_pack:
		tail = cur;

		++cur;
		cur %= E1000_RXDESC_NR;

	}
	REG_STORE(e1000_reg(dev, E1000_REG_RDT), tail);
}

static irq_return_t e1000_interrupt(unsigned int irq_num, void *dev_id) {
	int cause = REG_LOAD(e1000_reg(dev_id, E1000_REG_ICR));

	if (cause & (E1000_REG_ICR_RXO | E1000_REG_ICR_RXT)) {
		e1000_rx(dev_id);
	}

	if (cause & (E1000_REG_ICR_TXDW | E1000_REG_ICR_TXQE)) {
		txed_skb_clean(dev_id);
		e1000_xmit(dev_id);
	}

	if (cause & (E1000_REG_ICR_LSC)) {
		struct e1000_dev *dev = dev_id;

		dev->link_status ^= 1;

		if (dev->link_status) {
			printk("e1000: Link up\n");
		} else {
			printk("e1000: Link down. Please check and insert network cable\n");
		}

	}

	return IRQ_HANDLED;
}

static int e1000_open(struct net_device *_dev) {
	struct e1000_dev *dev = (struct e1000_dev *) _dev;

	mdelay(MDELAY);
	REG_ORIN(e1000_reg(dev, E1000_REG_CTRL), E1000_REG_CTRL_RST);

	mdelay(MDELAY);
	REG_ORIN(e1000_reg(dev, E1000_REG_CTRL), E1000_REG_CTRL_SLU | E1000_REG_CTRL_ASDE);
	REG_ANDIN(e1000_reg(dev, E1000_REG_CTRL), ~E1000_REG_CTRL_LRST);
	REG_ANDIN(e1000_reg(dev, E1000_REG_CTRL), ~E1000_REG_CTRL_PHY_RST);
	REG_ANDIN(e1000_reg(dev, E1000_REG_CTRL), ~E1000_REG_CTRL_ILOS);
	REG_STORE(e1000_reg(dev, E1000_REG_FCAL), 0);
	REG_STORE(e1000_reg(dev, E1000_REG_FCAH), 0);
	REG_STORE(e1000_reg(dev, E1000_REG_FCT), 0);
	REG_STORE(e1000_reg(dev, E1000_REG_FCTTV), 0);
	REG_ANDIN(e1000_reg(dev, E1000_REG_CTRL), ~E1000_REG_CTRL_VME);

	mdelay(MDELAY);
	/* Clear Multicast Table Array (MTA). */
	for (int i = 0; i < 128; i++) {
		volatile uint32_t *r = i + e1000_reg(dev, E1000_REG_MTA);
		REG_STORE(r, 0);
	}

	mdelay(MDELAY);
	/* Clear Multicast Table Array (MTA). */
#if 0 /*cleaned up on init */
	/* Initialize statistics registers. */
	for (int i = 0; i < 64; i++) {
		volatile uint32_t *r = i + e1000_reg(dev, E1000_REG_CRCERRS);
		prom_printf("0x%x\n", (unsigned int) r);
		REG_LOAD(r);
	}
#endif
	mdelay(MDELAY);
	REG_ORIN(e1000_reg(dev, E1000_REG_RCTL),  E1000_REG_RCTL_MPE);

	for (size_t i = 0; i < E1000_RXDESC_NR; i ++) {
		rx_descs[i].buffer_address = (uint32_t) rx_buf[i];
	}

	for (size_t i = 0; i < E1000_RXDESC_NR; i ++) {
		tx_descs[i].buffer_address = (uint32_t) rx_buf[i];
	}

	mdelay(MDELAY);
	REG_STORE(e1000_reg(dev, E1000_REG_RDBAL), (uint32_t) rx_descs);
	REG_STORE(e1000_reg(dev, E1000_REG_RDBAH), 0);
	REG_STORE(e1000_reg(dev, E1000_REG_RDLEN), sizeof(struct e1000_rx_desc) * E1000_RXDESC_NR);
	REG_STORE(e1000_reg(dev, E1000_REG_RDH), 0);
	REG_STORE(e1000_reg(dev, E1000_REG_RDT), E1000_RXDESC_NR - 1);
	REG_ORIN( e1000_reg(dev, E1000_REG_RCTL), E1000_REG_RCTL_EN);

	mdelay(MDELAY);
	REG_STORE(e1000_reg(dev, E1000_REG_TDBAL), (uint32_t) tx_descs);
	REG_STORE(e1000_reg(dev, E1000_REG_TDBAH), 0);
	REG_STORE(e1000_reg(dev, E1000_REG_TDLEN), sizeof(struct e1000_tx_desc) * E1000_TXDESC_NR);
	REG_STORE(e1000_reg(dev, E1000_REG_TDH), 0);
	REG_STORE(e1000_reg(dev, E1000_REG_TDT), 0);
	REG_ORIN(e1000_reg(dev, E1000_REG_TCTL), E1000_REG_TCTL_EN | E1000_REG_TCTL_PSP);

	mdelay(MDELAY);
	/* Enable interrupts. */
	REG_STORE(e1000_reg(dev, E1000_REG_IMS),
				      E1000_REG_IMS_RXO  |
				      E1000_REG_IMS_RXT  |
				      E1000_REG_IMS_TXQE |
				      E1000_REG_IMS_TXDW |
				      E1000_REG_IMS_LSC );
	return ENOERR;
}

static int stop(struct net_device *dev) {
	return ENOERR;
}

static int set_mac_address(struct net_device *_dev, const void *addr) {
	struct e1000_dev *dev = (struct e1000_dev *) _dev;
	REG_ANDIN(e1000_reg(dev, E1000_REG_RAH), ~E1000_REG_RAH_AV);

	REG_STORE(e1000_reg(dev, E1000_REG_RAL), *(uint32_t *) addr);
	REG_STORE(e1000_reg(dev, E1000_REG_RAH), *(uint16_t *) (addr + 4));

	REG_ORIN(e1000_reg(dev, E1000_REG_RAH), E1000_REG_RAH_AV);
	REG_ORIN(e1000_reg(dev, E1000_REG_RCTL),  E1000_REG_RCTL_MPE);

	memcpy(dev->dev.dev_addr, addr, ETH_ALEN);

	return ENOERR;
}

static const struct net_driver _drv_ops = {
	.xmit = xmit,
	.start = e1000_open,
	.stop = stop,
	.set_macaddr = set_mac_address
};

static struct e1000_dev *e1000_dev_alloc(void) {
	static struct e1000_dev pool, *ptr;

	if (ptr) {
		return NULL;
	}

	ptr = &pool;

	netdev_init((struct net_device *) ptr, "eth%u", &ethernet_setup);
	skb_queue_init(&ptr->txing_queue);
	ptr->link_status = 0;

	return ptr;
}

static int e1000_init(struct pci_slot_dev *pci_dev) {
	int res = 0;
	uint32_t nic_base;
	struct net_device *nic;

	nic_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;

	nic = (struct net_device *) e1000_dev_alloc();
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &_drv_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = nic_base;

	res = irq_attach(pci_dev->irq, e1000_interrupt, IF_SHARESUP, nic, "e1000");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}
