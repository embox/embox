/**
 * @file cadence_gem.c
 *
 * @date 06.03.20
 * @author: Nastya Nizharadze
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
#include <asm/regs.h>
#include <string.h>

#include <embox/unit.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <asm/io.h>

#include <stdio.h>
#include <kernel/printk.h>

#define BASE_ADDR			OPTION_GET(NUMBER,base_addr) // 0x10090000
#define SIFIVE_U_GEM_IRQ	(OPTION_GET(NUMBER,irq_num) + EXTERNAL_INTERRUPT_OFFSET)

#define PLIC_ADDR                  (uint32_t)OPTION_GET(NUMBER, plic_addr)
#define INTERRUPT_ENABLE_REG_1     (PLIC_ADDR + 0x2000U)
#define INTERRUPT_ENABLE_REG_2     (PLIC_ADDR + 0x2004U) //21st bit enable etnernet interrupt
#define ETH_IPL_ADDR               (PLIC_ADDR + 0xD4U) /* offset for hart 0 etnernet ipl */
#define ETH_IPL_ADDR_2             (PLIC_ADDR + 0xD8U) /* offset for hart 0 etnernet ipl */
#define ETH_IPL_ENABLE		0x00200000

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

#define MAX_BUFF_SIZE 1500

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

	riscv_read32(tx_stat, ((uint32_t)BASE_ADDR + (uint32_t)GEM_TXSTATUS));

	while ((!(tx_stat & GEM_TXSTATUS_TXCMPL)) & fst_xmit);

	fst_xmit = 1;

	riscv_read32(tx_stat, ((uint32_t)BASE_ADDR + (uint32_t)GEM_TXSTATUS));
	tx_stat |= GEM_TXSTATUS_TXCMPL;
	riscv_write32(tx_stat, ((uint32_t)BASE_ADDR + (uint32_t)GEM_TXSTATUS));

	dma_desc_tx.flags |= DESC_1_USED;
	dma_desc_tx.flags &= ~DESC_1_LENGTH;

	memset(tx_buff, 0, sizeof(char) * MAX_BUFF_SIZE);
	memcpy(tx_buff, skb->mac.raw, skb->len);

	dma_desc_tx.addr = (uint32_t) tx_buff;
	dma_desc_tx.flags |= (skb->len);
	
	//show_packet(skb->mac.raw, skb->len, "xmit");

	dma_desc_tx.flags &= ~DESC_1_USED;
	riscv_read32(net_control, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL));
	net_control |= GEM_NWCTRL_TXSTART;
	riscv_write32(net_control, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL));

	skb_free(skb);
	return 0;
}

static int cadence_gem_start(struct net_device *dev) {
	uint32_t net_control;
	uint32_t net_config;
	uint32_t net_ier;
	unsigned char mac_addr[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x02};

	riscv_write32((uint32_t)*(uint32_t *)mac_addr, ((uint32_t)BASE_ADDR + (uint32_t)GEM_SPADDR1LO));
	riscv_write32((uint16_t)*(uint16_t *)(mac_addr + 4), ((uint32_t)BASE_ADDR + (uint32_t)GEM_SPADDR1HI));

	dma_desc_tx.flags = DESC_1_USED & DESC_1_TX_WRAP & DESC_1_TX_LAST;
	dma_desc_tx.addr = 0;

	dma_desc_rx.addr = (uint32_t)rx_buff | DESC_0_RX_WRAP;
	dma_desc_rx.flags = 0; 

	riscv_read32(net_config, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCFG));
	net_config |= GEM_NWCFG_STRIP_FCS;
	riscv_write32(net_config, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCFG));

	riscv_read32(net_ier, ((uint32_t)BASE_ADDR + (uint32_t)GEM_IER));
	net_ier |= GEM_INT_TXCMPL & GEM_INT_TXUSED & GEM_INT_RXUSED & GEM_INT_RXCMPL;
	riscv_write32(net_ier, ((uint32_t)BASE_ADDR + (uint32_t)GEM_IER));

	riscv_write32(&dma_desc_rx, ((uint32_t)BASE_ADDR + (uint32_t)GEM_RXQBASE));
	riscv_write32(&dma_desc_tx, ((uint32_t)BASE_ADDR + (uint32_t)GEM_TXQBASE));

	riscv_read32(net_control, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL));
	net_control |= GEM_NWCTRL_TXENA;
	net_control |= GEM_NWCTRL_RXENA;
	riscv_write32(net_control, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL));

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

	skb = skb_alloc(dma_desc_rx.flags & DESC_1_LENGTH);

	if (!skb) {
			dev->stats.rx_dropped++;
			return -ENOMEM;
	}

	skb->len = (dma_desc_rx.flags & DESC_1_LENGTH);

	memcpy(skb->mac.raw, rx_buff, skb->len);

	dma_desc_rx.addr = (uint32_t)rx_buff | DESC_0_RX_WRAP;
	dma_desc_tx.flags &= ~DESC_1_LENGTH;

	riscv_read32(net_control, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL));
	net_control |= GEM_NWCTRL_RXENA;
	riscv_write32(net_control, ((uint32_t)BASE_ADDR + (uint32_t)GEM_NWCTRL));

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
	
	cadence_gem_rx(dev);
	
	return 0;
}

static int cadence_gem_init(void) {
	struct net_device *netdev;
	int err;

	uint32_t *eth_ipl = (uint32_t *)(ETH_IPL_ADDR);
	uint32_t *eth_interrupt_enable = (uint32_t *)(INTERRUPT_ENABLE_REG_2);

	*eth_ipl = 0x1; // priority levels are from 1 to 7. 0 disable interrupt
	*eth_interrupt_enable = ETH_IPL_ENABLE; // 21st bit enable etnernet interrupt

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
