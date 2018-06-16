/**
 * @file
 * @brief Device driver for RealTek RTL8139 Fast Ethernet.
 *
 * @date 18.12.11
 * @author Nikolay Korotkiy
 *         - Initial implementation
 */

#include <asm/io.h>
#include <embox/unit.h>
#include <errno.h>
#include <drivers/pci/pci.h>
#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include <arpa/inet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <drivers/ethernet/rtl8139.h>
#include <net/l0/net_entry.h>

PCI_DRIVER("rtl8139", rtl8139_init, PCI_VENDOR_ID_REALTEK, PCI_DEV_ID_REALTEK_8139);


#define RX_BUF_SIZE (32 * 1024)
#define TX_BUF_SIZE 1536

static uint8_t rx_buf[RX_BUF_SIZE];
static uint8_t tx_buf[4][TX_BUF_SIZE] __attribute__((aligned(4)));

static uint32_t cur_tx;
static uint32_t cur_rx;

static int xmit(struct net_device *dev, struct sk_buff *skb) {
	uint32_t entry   = cur_tx % 4;
	uint32_t size    = skb->len;
	uint8_t *msg_buf = tx_buf[entry];

	memcpy(msg_buf, skb->mac.raw, skb->len);

	out32(((256 << 11) & 0x003f0000) | size,
		dev->base_addr + RTL8139_TX_STAT_0 + (entry * 4));
	cur_tx++;

	skb_free(skb); /* free packet */
	return ENOERR;
}

static inline void wrap_copy(uint8_t *dst, const unsigned char *ring,
				uint32_t offset, size_t size) {
	uint32_t left = RX_BUF_SIZE - offset;

	if (size > left) {
		memcpy(dst, ring + offset, left);
		memcpy(dst + left, ring, size - left);
	} else {
		memcpy(dst, ring + offset, size);
	}
}

static void rtl8139_rx(struct net_device *dev) {
	net_device_stats_t *stat = &(dev->stats);
	unsigned long base_addr = dev->base_addr;

	while ((in8(base_addr + RTL8139_CR) & CR_TX_BUF_EMPTY) == 0) {
		struct sk_buff *skb;

		uint16_t ring_offset = cur_rx % RX_BUF_SIZE;
		uint8_t *buf_ptr = (uint8_t *) (rx_buf + ring_offset);

		uint16_t rx_size =  *(uint16_t *) (buf_ptr + 2);
		uint16_t pkt_len = rx_size - 4;

		skb = skb_alloc(pkt_len);
		assert(skb);
		wrap_copy(skb->mac.raw, rx_buf, (ring_offset + 4) % RX_BUF_SIZE, skb->len);
		skb->dev = dev;

		stat->rx_packets++;
		stat->rx_bytes += skb->len;

		cur_rx = ((cur_rx + rx_size + 4 + 3) % RX_BUF_SIZE) & ~0x3;
		out16((uint16_t) (cur_rx - 16), base_addr + RTL8139_RX_BUFP);

		out16(ISR_RX_ACK_BITS, base_addr + RTL8139_ISR); /* clear */
		netif_rx(skb);
	}
}

static irq_return_t rtl8139_interrupt(unsigned int irq_num, void *dev_id) {
	unsigned long base_addr = ((struct net_device *) dev_id)->base_addr;
	uint16_t status = in16(base_addr + RTL8139_ISR);

	if (status & PKT_RX) {
		rtl8139_rx((struct net_device *) dev_id);
		out16(status, base_addr + RTL8139_ISR);
	} else {
		out16(TX_OK, base_addr + RTL8139_ISR);
	}
	return IRQ_HANDLED;
}

static int open(struct net_device *dev) {
	cur_tx = 0;
	cur_rx = 0;

	/* Reset the chip */
	out8(CR_RESET, dev->base_addr + RTL8139_CR);
	/* Unlock Config[01234] and BMCR register writes */
	out8(CFG9346_UNLOCK, dev->base_addr + RTL8139_9346CR);
	/* Enable Tx/Rx before setting transfer thresholds */
	out8(CR_RX_ENABLE | CR_TX_ENABLE, dev->base_addr + RTL8139_CR);
	/* Using 32K ring */
	out32(RCR_RCV_32K | RCR_NO_WRAP | RCR_FIFO_NONE | RCR_DMA_UNLIM | \
		RCR_AC_BROADCAST | RCR_AC_MYPHYS,
		dev->base_addr + RTL8139_RCR);
	out32(TCR_IFG96 | (6 << TCR_DMA_SHIFT) | (8 << TCR_RETRY_SHIFT),
		dev->base_addr + RTL8139_TCR);
	/* Lock Config[01234] and BMCR register writes */
	out8(CFG9346_LOCK, dev->base_addr + RTL8139_9346CR);

	/* init Rx ring buffer DMA address */
	out32((uint32_t) rx_buf, dev->base_addr + RTL8139_RX_BUF);
	/* init Tx buffer DMA addresses (4 registers) */
	for (int i = 0; i < 4; i++) {
		out32((uint32_t) tx_buf[i],
			dev->base_addr + RTL8139_TX_ADDR_0 + (i * 4));
	}

	out32(0, dev->base_addr + RTL8139_MPC); /* missed packet counter */
	out32(in32(dev->base_addr + RTL8139_RCR) | RCR_AC_BROADCAST | \
		RCR_AC_MULTICAST | RCR_AC_MYPHYS | RCR_AC_ALLPHYS,
		dev->base_addr + RTL8139_RCR);

	for (int i = 0; i < 8; i++) {
		out8(0xFF, dev->base_addr + RTL8139_MAR0 + i);
	}

	for (int i = 0; i < ETH_ALEN; i++) {
		dev->dev_addr[i] = in8(dev->base_addr + RTL8139_MAC0);
	}

	out16(in16(dev->base_addr + RTL8139_MULT_INTR) & MULT_INTR_CLEAR,
		dev->base_addr + RTL8139_MULT_INTR); /* no early-rx interrupts */

	/* make sure RxTx has started */
	if (!(in8(dev->base_addr + RTL8139_CR) & CR_RX_ENABLE) ||
	    !(in8(dev->base_addr + RTL8139_CR) & CR_TX_ENABLE)) {
		out8(CR_RX_ENABLE | CR_TX_ENABLE, dev->base_addr + RTL8139_CR);
	}

	/* Enable interrupts */
	out16(ISR_PCI_ERR | ISR_PCSTIMEOUT | ISR_RX_UNDERRUN | ISR_RX_OVERFLOW | \
		ISR_RX_FIFOOVER | ISR_TX_ERR | ISR_TX_OK | ISR_RX_ERR | ISR_RX_OK,
		dev->base_addr + RTL8139_IMR);
	return ENOERR;
}

static int stop(struct net_device *dev) {
	/* Disable interrupts */
	out16(0x0000, dev->base_addr + RTL8139_IMR);
	/* Stop the chip's Tx and Rx DMA processes */
	out8(0x00, dev->base_addr + RTL8139_CR);

	out32(0, dev->base_addr + RTL8139_MPC);
	out8(CFG9346_UNLOCK, dev->base_addr + RTL8139_9346CR);

	return ENOERR;
}

static int set_mac_address(struct net_device *dev, const void *addr) {
	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	for (size_t i = 0; i < ETH_ALEN; i++) {
		out8(*((uint8_t *) addr + i), dev->base_addr + RTL8139_MAR0 + i);
	}
	memcpy(dev->dev_addr, addr, ETH_ALEN);

	return ENOERR;
}

static const struct net_driver _drv_ops = {
	.xmit = xmit,
	.start = open,
	.stop = stop,
	.set_macaddr = set_mac_address
};

static int rtl8139_init(struct pci_slot_dev *pci_dev) {
	int res = 0;
	uint32_t nic_base;
	struct net_device *nic;

	assert(pci_dev != NULL);

	nic_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &_drv_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = nic_base;

	res = irq_attach(pci_dev->irq, rtl8139_interrupt, 0, nic, "rtl-8139");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}
