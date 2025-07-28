/**
 * @file cadence_gem.c
 *
 * @date 06.03.20
 * @author: Nastya Nizharadze
 *
 * Reference: Zynq-7000 All Programmable SoC Technical Reference Manual.
 */

#include <errno.h>
#include <stdint.h>
#include <util/log.h>

#include <kernel/irq.h>
#include <net/skbuff.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/inetdevice.h>
#include <net/util/show_packet.h>
#include <asm/csr.h>
#include <string.h>

#include <embox/unit.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <asm/io.h>
#include <hal/reg.h>

#include <stdio.h>
#include <kernel/printk.h>

#define BASE_ADDR			OPTION_GET(NUMBER,base_addr)
#define SIFIVE_U_GEM_IRQ	OPTION_GET(NUMBER,irq_num)

#define GEM_NWCTRL        0x00000000 /* Network Control */
#define GEM_NWCFG         0x00000004 /* Network Config */
#define GEM_NWSTATUS      0x00000008 /* Network Status */
#define GEM_USERIO        0x0000000C /* User IO */
#define GEM_DMACFG        0x00000010 /* DMA Control */
#define GEM_TXSTATUS      0x00000014 /* TX Status */
#define GEM_RXQBASE       0x00000018 /* RX Q Base address */
#define GEM_TXQBASE       0x0000001C /* TX Q Base address */
#define GEM_RXSTATUS      0x00000020 /* RX Status */
#define GEM_ISR           0x00000024 /* Interrupt Status */
#define GEM_IER           0x00000028 /* Interrupt Enable */
#define GEM_IDR           0x0000002C /* Interrupt Disable */
#define GEM_IMR           0x00000030 /* Interrupt Mask */
#define GEM_PHYMNTNC      0x00000034 /* Phy Maintenance */

#define GEM_SPADDR1LO     0x00000088 /* Specific addr 1 low reg */
#define GEM_SPADDR1HI     0x0000008C /* Specific addr 1 high reg */

#define GEM_TXSTATUS_TXCMPL    0x00000020 /* Transmit Complete */
#define GEM_TXSTATUS_USED      0x00000001

#define GEM_NWCTRL_TXENA       0x00000008 /* Transmit Enable */
#define GEM_NWCTRL_TXSTART     0x00000200 /* Transmit Enable */
#define GEM_NWCTRL_RXENA       0x00000004 /* Receive Enable */

#define GEM_NWCFG_STRIP_FCS    0x00020000 /* Strip FCS field */

#define DESC_1_USED		0x80000000
#define DESC_1_LENGTH	0x00001FFF

#define DESC_1_TX_WRAP	0x40000000
#define DESC_1_TX_LAST	0x00008000

#define DESC_0_RX_WRAP		0x00000002
#define DESC_0_RX_OWNERSHIP	0x00000001

#define GEM_INT_TXCMPL	0x00000080 /* Transmit Complete */
#define GEM_INT_TXUSED	0x00000008
#define GEM_INT_RXUSED	0x00000004
#define GEM_INT_RXCMPL	0x00000002

#define GEM_TXDESC_LAST_BUF			(1 << 15)  /* last in frame */
#define GEM_TXDESC_USED			(1U << 31) /* done txmitting */
#define GEM_TXDESC_WRAP			(1 << 30)  /* end descr ring */
#define GEM_RXDESC_EOF				(1 << 15) /* end of frame */
#define GEM_RXDESC_SOF				(1 << 14) /* start of frame */
#define GEM_INTR_STAT			0x024	/* Interrupt Status */
#define GEM_RX_STAT			0x020	/* Receive Status */
#define GEM_DMA_CFG_RX_BUF_SIZE(sz)		((((sz) + 63) / 64)  <<  16)

#define MAX_BUFF_SIZE 1600

int fst_xmit = 0;

struct dma_desc_rx {
	uint32_t addr;
	uint32_t flags;
} dma_desc_rx __attribute__((aligned(4)));

struct dma_desc_tx {
	uint32_t addr;
	uint32_t flags;
} dma_desc_tx __attribute__((aligned(4)));


unsigned char rx_buff[MAX_BUFF_SIZE] __attribute__((aligned(4)));
unsigned char tx_buff[MAX_BUFF_SIZE] __attribute__((aligned(4)));

static int cadence_gem_xmit(struct net_device *dev, struct sk_buff *skb) {
  uint32_t net_control;
  uint32_t tx_stat;

  tx_stat = REG32_LOAD((uint32_t)BASE_ADDR + (uint32_t)GEM_TXSTATUS);
  tx_stat |= GEM_TXSTATUS_TXCMPL;
  REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_TXSTATUS), tx_stat);

  memset(tx_buff, 0, sizeof(char) * MAX_BUFF_SIZE);
  memcpy(tx_buff, skb->mac.raw, skb->len);

  dma_desc_tx.addr = (uint32_t) tx_buff;
  dma_desc_tx.flags &= 0xffffc000;
  dma_desc_tx.flags |= (skb->len);

  dma_desc_tx.flags |= GEM_TXDESC_LAST_BUF;
  dma_desc_tx.flags &= ~GEM_TXDESC_USED;

  //show_packet(skb->mac.raw, skb->len, "xmit");

  net_control = REG32_LOAD((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL);
  net_control |= GEM_NWCTRL_TXSTART;
  REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL), net_control);

  skb_free(skb);
  return 0;
}

static void program_dma_cfg(void) {
	uint32_t val = 0;

	val |= GEM_DMA_CFG_RX_BUF_SIZE(MAX_BUFF_SIZE);
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_DMACFG), val);
}

static int cadence_gem_start(struct net_device *dev) {
	uint32_t net_control;
	uint32_t net_config;
	uint32_t net_ier;
	unsigned char mac_addr[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x02};

	program_dma_cfg();

	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_SPADDR1LO), (uint32_t)*(uint32_t *)mac_addr);
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_SPADDR1HI), (uint16_t)*(uint16_t *)(mac_addr + 4));

	dma_desc_tx.flags = DESC_1_USED & DESC_1_TX_WRAP & DESC_1_TX_LAST;
	dma_desc_tx.addr = 0;
	dma_desc_tx.flags = 0;
	dma_desc_tx.flags = (0u << 31) | GEM_TXDESC_WRAP;

	dma_desc_rx.addr = (uint32_t)rx_buff | DESC_0_RX_WRAP;
	dma_desc_rx.flags = 0;

	net_config = REG32_LOAD((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCFG);
	net_config |= GEM_NWCFG_STRIP_FCS;
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCFG), net_config);

	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_RXQBASE), (uint32_t)&dma_desc_rx);
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_TXQBASE), (uint32_t)&dma_desc_tx);

	net_ier =REG32_LOAD((uint32_t)BASE_ADDR + (uint32_t)GEM_IER);
	net_ier |= GEM_INT_TXCMPL | GEM_INT_TXUSED | GEM_INT_RXUSED | GEM_INT_RXCMPL;
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_IER), net_ier);

	net_control = REG32_LOAD((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL);
	net_control |= GEM_NWCTRL_TXENA;
	net_control |= GEM_NWCTRL_RXENA;
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL), net_control);

	return 0;
}

static int cadence_gem_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver cadence_gem_drv_ops = {
	.xmit = cadence_gem_xmit,
	.start = cadence_gem_start,
	.set_macaddr = cadence_gem_set_macaddr
};

static int cadence_gem_rx(struct net_device *dev) {
	uint32_t net_control;
	struct sk_buff *skb;

	if (!(dma_desc_rx.addr & DESC_0_RX_OWNERSHIP)){
		dma_desc_tx.flags &= ~DESC_1_LENGTH;
		dma_desc_rx.addr &= ~DESC_0_RX_OWNERSHIP;
		return 0;
	}

	uint32_t ctl = dma_desc_rx.flags;
	if ((ctl & (GEM_RXDESC_SOF | GEM_RXDESC_EOF)) !=
		(GEM_RXDESC_SOF | GEM_RXDESC_EOF)) {
		return 0;
	}

	skb = skb_alloc(dma_desc_rx.flags & DESC_1_LENGTH);

	if (!skb) {
			dev->stats.rx_dropped++;
			return -ENOMEM;
	}

	skb->len = (dma_desc_rx.flags & DESC_1_LENGTH);

	memcpy(skb->mac.raw, rx_buff, skb->len);

	dma_desc_rx.addr = (uint32_t)rx_buff | DESC_0_RX_WRAP;
	dma_desc_tx.flags &= ~DESC_1_LENGTH;

	net_control = REG32_LOAD((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL);
	net_control |= GEM_NWCTRL_RXENA;
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL), net_control);

	//show_packet(skb->mac.raw, skb->len, "rx");
	skb->dev = dev;
	netif_rx(skb);

	dev->stats.rx_packets++;
	dev->stats.rx_bytes += skb->len;

	return 0;
}

static irq_return_t cadence_gem_interrupt_handler(unsigned int irq,
												  void *dev_id) {
	struct net_device *dev = dev_id;
	uint32_t istatus;

	/* Read and clear registers */
	istatus = REG32_LOAD((uint32_t)BASE_ADDR + (uint32_t)GEM_INTR_STAT);
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_INTR_STAT), istatus);
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_RX_STAT), istatus);
	REG32_STORE(((uint32_t)BASE_ADDR + (uint32_t)GEM_RX_STAT), 0xf);

	cadence_gem_rx(dev);

	return 0;
}

static int cadence_gem_init(void) {
	struct net_device *netdev;
	int err;

	netdev = etherdev_alloc(0);
	if (!netdev) {
		return -ENOMEM;
	}

	netdev->base_addr = BASE_ADDR;
	netdev->irq = SIFIVE_U_GEM_IRQ;
	netdev->drv_ops = &cadence_gem_drv_ops;

	err = irq_attach(SIFIVE_U_GEM_IRQ, cadence_gem_interrupt_handler, 
									 0, netdev, "cadence_gem");
	if (err) {
		return err;
	}

	return inetdev_register_dev(netdev);
}

EMBOX_UNIT_INIT(cadence_gem_init);
