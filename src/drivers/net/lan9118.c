/**
 * @file
 * @brief Ethernet Controller SMSC LAN9118
 *
 * @date 30.10.13
 * @author Alexander Kalmuk
 */

#include <drivers/gpmc.h>
#include <drivers/pins.h>
#include <errno.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <hal/pins.h>
#include <framework/mod/options.h>
#include <kernel/printk.h>

#include <net/l2/ethernet.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/netfilter.h>

#define LAN9118_GPMC_CS        5 // GPMC chip-select number
#define LAN9118_PIN            OPTION_GET(NUMBER, pin)
#define SIZE_16M               0x1000000

/* CONTROL AND STATUS REGISTERS */
#define LAN9118_ID_REV         0x50
#define LAN9118_IRQ_CFG        0x54

#define _LAN9118_IRQ_CFG_EN         0x00000100
#define _LAN9118_IRQ_CFG_POL        0x00000010
#define _LAN9118_IRQ_CFG_TYPE       0x00000001

#define LAN9118_INT_STS        0x58
#define LAN9118_INT_EN         0x5C
#define LAN9118_BYTE_TEST      0x64
#define LAN9118_FIFO_INT       0x68
#define LAN9118_RX_CFG         0x6C
#define LAN9118_TX_CFG         0x70
#define LAN9118_HW_CFG         0x74
#define LAN9118_RX_DP_CTL      0x78
#define LAN9118_RX_FIFO_INF    0x7C
#define LAN9118_TX_FIFO_INF    0x80
#define LAN9118_PMT_CTRL       0x84
#define LAN9118_GPIO_CFG       0x88
#define LAN9118_GPT_CFG        0x8C
#define LAN9118_GPT_CNT        0x90
#define LAN9118_WORD_SWAP      0x98
#define LAN9118_FREE_RUN       0x9C
#define LAN9118_RX_DROP        0xA0
#define LAN9118_MAC_CSR_CMD    0xA4
#define LAN9118_MAC_CSR_DATA   0xA8
#define LAN9118_AFC_CFG        0xAC
#define LAN9118_E2P_CMD        0xB0
#define LAN9118_E2P_DATA       0xB4


EMBOX_UNIT_INIT(lan9118_init);

static uint32_t lan9181_reg_read(struct net_device *dev, int offset) {
	return REG_LOAD(dev->base_addr + offset);
}

static void lan9181_reg_write(struct net_device *dev, int offset, uint32_t mask) {
	REG_STORE(dev->base_addr + offset, mask);
}

void lan9118_irq_handler(pin_mask_t ch_mask, pin_mask_t mon_mask) {
	(void) ch_mask;
	(void) mon_mask;

	printk("lan9118 irq\n");
}

static void lan9118_reset(struct net_device *dev) {
	uint32_t irq_cfg;

	// XXX hw reset

	/* Initialize interrupts */
	lan9181_reg_write(dev, LAN9118_INT_EN, 0);
	lan9181_reg_write(dev, LAN9118_INT_STS, -1);

	irq_cfg = (1 << 24) | _LAN9118_IRQ_CFG_POL | _LAN9118_IRQ_CFG_TYPE;
	lan9181_reg_write(dev, LAN9118_IRQ_CFG, irq_cfg);
}

static void lan9181_enable(struct net_device *dev) {
	pin_set_input_monitor((1L << LAN9118_PIN), lan9118_irq_handler);
}

static int lan9118_init(void) {
	int res;
	struct net_device *nic;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	res = gpmc_cs_init(LAN9118_GPMC_CS, (uint32_t *) &nic->base_addr, SIZE_16M);

	if (res < 0)
		return -1;

	if (lan9181_reg_read(nic, LAN9118_BYTE_TEST) != 0x87654321) {
		printk("%d\n", lan9181_reg_read(nic, LAN9118_BYTE_TEST));
		return -1;
	}

	lan9118_reset(nic);
	lan9181_enable(nic);

	return 0;
}
