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
#include <sys/mman.h>

#include <asm/io.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/netfilter.h>

#include <net/l0/net_entry.h>

#include <hal/reg.h>

#include <drivers/net/e1000.h>

#include <mem/misc/pool.h>

#include <kernel/printk.h>

#include <kernel/panic.h>

#include <embox/unit.h>

static const struct pci_id e1000_id_table[] = {
	{ PCI_VENDOR_ID_INTEL, PCI_DEV_ID_INTEL_82540EM },
	{ PCI_VENDOR_ID_INTEL, PCI_DEV_ID_INTEL_82567V3 },
};

PCI_DRIVER_TABLE("e1000", e1000_init, e1000_id_table);

#define MDELAY 1000

/** Number of receive descriptors per card. */
#define E1000_RXDESC_NR 16

/** Number of transmit descriptors per card. */
#define E1000_TXDESC_NR 16

/** Size of each I/O buffer per descriptor. */
#define E1000_IOBUF_SIZE 2048

#define E1000_RX_CHECKSUM_LEN 4

#define E1000_MAX_RX_LEN (ETH_FRAME_LEN + E1000_RX_CHECKSUM_LEN)

struct e1000_priv {
	struct sk_buff_head txing_queue;
	struct sk_buff_head tx_dev_queue;
	char link_status;
};

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
static struct sk_buff *rx_skbs[E1000_RXDESC_NR];
static void mdelay(int value) {
	volatile int delay = value;

	while (delay --);

}

static volatile uint32_t *e1000_reg(struct net_device *dev, int offset) {
	return (volatile uint32_t *) (dev->base_addr + offset);
}

static int e1000_xmit(struct net_device *dev) {
	uint16_t head;
	uint16_t tail;
	struct sk_buff *skb;

	/* Called from kernel space and IRQ. Don't want tail to be handled twice */
	irq_lock();
	{
		head = REG_LOAD(e1000_reg(dev, E1000_REG_TDH));
		tail = REG_LOAD(e1000_reg(dev, E1000_REG_TDT));

		if ((tail + 1 % E1000_TXDESC_NR) == head) {
			goto out_unlock;
		}

		skb = skb_queue_pop(&netdev_priv(dev, struct e1000_priv)->tx_dev_queue);

		if (skb == NULL) {
			goto out_unlock;
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

		skb_queue_push(&netdev_priv(dev, struct e1000_priv)->txing_queue, skb);
	}
out_unlock:
	irq_unlock();
	return 0;
}

static int xmit(struct net_device *dev, struct sk_buff *skb) {

	irq_lock();
	{
		skb_queue_push(&netdev_priv(dev, struct e1000_priv)->tx_dev_queue,
				skb);
	}
	irq_unlock();

	e1000_xmit(dev);

	return ENOERR;
}

static void txed_skb_clean(struct net_device *dev) {
	struct sk_buff *skb;

	irq_lock();
	{
		while ((skb = skb_queue_pop(&netdev_priv(dev, struct e1000_priv)->txing_queue))) {
			skb_free(skb);
		}
	}
	irq_unlock();
}

static void e1000_rx(struct net_device *dev) {
	/*net_device_stats_t stat = get_eth_stat(dev);*/
	uint16_t head;
	uint16_t tail;
	uint16_t cur;

	struct sk_buff *skb;

	/* nested irq locked to prevent already handled packets handled once more
	 * as tail updated at exit
	 */
	irq_lock();
	{
		head = REG_LOAD(e1000_reg(dev, E1000_REG_RDH));
		tail = REG_LOAD(e1000_reg(dev, E1000_REG_RDT));
		cur = (1 + tail) % E1000_RXDESC_NR;

		while (cur != head) {
			int len;

			if (!(rx_descs[cur].status)) {
				break;
			}


			if (0 != nf_test_raw(NF_CHAIN_INPUT, NF_TARGET_ACCEPT, (void *) rx_descs[cur].buffer_address,
						ETH_ALEN + (void *) rx_descs[cur].buffer_address, ETH_ALEN)) {
				goto drop_pack;
			}

			skb = rx_skbs[cur];
			assert (skb);

			rx_skbs[cur] = skb_alloc(E1000_MAX_RX_LEN);
			if (rx_skbs[cur] == NULL)
				panic("Can't allocate rx_skbs %d", cur);
			rx_descs[cur].buffer_address = (uint32_t) rx_skbs[cur]->mac.raw;

			len = rx_descs[cur].length - E1000_RX_CHECKSUM_LEN;
			skb = skb_realloc(len, skb);
			if (!skb) {
				goto drop_pack;
			}

			skb->dev = dev;
			netif_rx(skb);
drop_pack:
			tail = cur;

			cur = (1 + tail) % E1000_RXDESC_NR;
		}
		REG_STORE(e1000_reg(dev, E1000_REG_RDT), tail);
	}
	irq_unlock();
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
		struct net_device *dev = dev_id;

		netdev_priv(dev, struct e1000_priv)->link_status ^= 1;

		if (netdev_priv(dev, struct e1000_priv)->link_status) {
			printk("e1000: Link up\n");
			netdev_flag_up(dev, IFF_RUNNING);
		} else {
			printk("e1000: Link down. Please check and insert network cable\n");
			netdev_flag_down(dev, IFF_RUNNING);
		}
	}

	return IRQ_HANDLED;
}

static int e1000_open(struct net_device *dev) {

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
		printk("0x%x\n", (unsigned int) r);
		REG_LOAD(r);
	}
#endif
	mdelay(MDELAY);
	REG_ORIN(e1000_reg(dev, E1000_REG_RCTL),  E1000_REG_RCTL_MPE);

	for (size_t i = 0; i < E1000_RXDESC_NR; i ++) {
	        struct sk_buff *skb;
		skb = skb_alloc(E1000_MAX_RX_LEN);
		if (skb == NULL)
			panic("Can't allocate skb %d", i);
		rx_skbs[i]= skb;
		rx_descs[i].buffer_address = (uint32_t) skb->mac.raw;
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

static int set_mac_address(struct net_device *dev, const void *addr) {
	REG_ANDIN(e1000_reg(dev, E1000_REG_RAH), ~E1000_REG_RAH_AV);

	REG_STORE(e1000_reg(dev, E1000_REG_RAL), *(uint32_t *) addr);
	REG_STORE(e1000_reg(dev, E1000_REG_RAH), *(uint16_t *) (addr + 4));

	REG_ORIN(e1000_reg(dev, E1000_REG_RAH), E1000_REG_RAH_AV);
	REG_ORIN(e1000_reg(dev, E1000_REG_RCTL),  E1000_REG_RCTL_MPE);

	memcpy(dev->dev_addr, addr, ETH_ALEN);

	return ENOERR;
}

static const struct net_driver _drv_ops = {
	.xmit = xmit,
	.start = e1000_open,
	.stop = stop,
	.set_macaddr = set_mac_address
};

static void e1000_enable_bus_mastering(struct pci_slot_dev *pci_dev) {
	const uint8_t devfn = PCI_DEVFN(pci_dev->slot, pci_dev->func);
	uint16_t pci_command;

	pci_read_config16(pci_dev->busn, devfn, PCI_COMMAND, &pci_command);
	pci_write_config16(pci_dev->busn, devfn, PCI_COMMAND, pci_command | PCI_COMMAND_MASTER);
}

static int e1000_init(struct pci_slot_dev *pci_dev) {
	int res;
	struct net_device *nic;
	struct e1000_priv *nic_priv;

	nic = (struct net_device *) etherdev_alloc(sizeof *nic_priv);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &_drv_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = (uintptr_t) mmap_device_memory(
			(void *) (pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK),
			0x6000, /* XXX */
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK);
	nic_priv = netdev_priv(nic, struct e1000_priv);
	skb_queue_init(&nic_priv->txing_queue);
	skb_queue_init(&nic_priv->tx_dev_queue);
	nic_priv->link_status = 0;

	res = irq_attach(pci_dev->irq, e1000_interrupt, IF_SHARESUP, nic, "e1000");
	if (res < 0) {
		return res;
	}

	e1000_enable_bus_mastering(pci_dev);

	return inetdev_register_dev(nic);
}
