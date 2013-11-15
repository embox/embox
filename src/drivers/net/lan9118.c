/**
 * @file
 * @brief Ethernet Controller SMSC LAN9118
 *
 * @date 30.10.13
 * @author Alexander Kalmuk
 */

#include <drivers/gpmc.h>
#include <drivers/gpio.h>
#include <errno.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <hal/pins.h>
#include <kernel/printk.h>

#include <net/l2/ethernet.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/netfilter.h>
#include <unistd.h>

#define LAN9118_GPMC_CS        5 /* GPMC chip-select number */
#define LAN9118_PORT           OPTION_GET(NUMBER, port)
#define LAN9118_PIN            OPTION_GET(NUMBER, irq_pin)
#define SIZE_16M               0x1000000

/* CONTROL AND STATUS REGISTERS */
#define LAN9118_ID_REV         0x50

#define LAN9118_IRQ_CFG        0x54
#define _LAN9118_IRQ_CFG_EN                0x00000100
#define _LAN9118_IRQ_CFG_POL               0x00000010
#define _LAN9118_IRQ_CFG_TYPE              0x00000001

#define LAN9118_INT_STS        0x58
#define _LAN9118_INT_STS_RXSTOP_INT_       0x01000000

#define LAN9118_INT_EN         0x5C
#define _LAN9118_INT_EN_SW                 0x80000000
#define _LAN9118_INT_EN_RSFL               0x00000008
#define _LAN9118_INT_EN_RXSTOP_INT         0x01000000

#define LAN9118_BYTE_TEST      0x64

#define LAN9118_FIFO_INT       0x68
#define _LAN9118_FIFO_INT_TX_AVAIL_LEVEL_  0xFF000000
#define _LAN9118_FIFO_INT_TX_STS_LEVEL_    0x00FF0000
#define _LAN9118_FIFO_INT_RX_AVAIL_LEVEL_  0x0000FF00
#define _LAN9118_FIFO_INT_RX_STS_LEVEL_    0x000000FF

#define LAN9118_RX_CFG         0x6C
#define LAN9118_TX_CFG         0x70

#define LAN9118_HW_CFG         0x74
#define _LAN9118_HW_CFG_SF_                0x00100000
#define _LAN9118_HW_CFG_TX_FIF_SZ_         0x000F0000
#define _LAN9118_HW_CFG_SRST               0x00000001

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
#define _LAN9118_MAC_CSR_CMD_CSR_BUSY      0x80000000
#define _LAN9118_MAC_CSR_CMD_R_NOT_W       0x40000000
#define _LAN9118_MAC_CSR_CMD_CSR_ADDR      0x000000FF


#define LAN9118_MAC_CSR_DATA   0xA8
#define LAN9118_AFC_CFG        0xAC
#define LAN9118_E2P_CMD        0xB0
#define LAN9118_E2P_DATA       0xB4

/* MAC */
#define LAN9118_MAC_CR         0x01
#define _LAN9118_MAC_CR_HBDIS              0x10000000
#define _LAN9118_MAC_CR_TXEN               0x00000008
#define _LAN9118_MAC_CR_RXEN               0x00000004

EMBOX_UNIT_INIT(lan9118_init);

static struct net_device *nic;

static uint32_t lan9118_reg_read(struct net_device *dev, int offset) {
	return REG_LOAD(dev->base_addr + offset);
}

static void lan9118_reg_write(struct net_device *dev, int offset, uint32_t mask) {
	REG_STORE(dev->base_addr + offset, mask);
}

static uint32_t lan9118_mac_read(struct net_device *dev, unsigned int offset) {
	uint32_t l;

	l = lan9118_reg_read(dev, LAN9118_MAC_CSR_CMD);

	if (l & _LAN9118_MAC_CSR_CMD_CSR_BUSY) {
		printk("MAC is busy\n");
		return 0xFFFFFFFF;
	}

	lan9118_reg_write(dev, LAN9118_MAC_CSR_CMD, ((offset & 0xFF) |
			_LAN9118_MAC_CSR_CMD_CSR_BUSY | _LAN9118_MAC_CSR_CMD_R_NOT_W));

	/* Workaround for hardware read-after-write restriction. */
	l = lan9118_reg_read(dev, LAN9118_BYTE_TEST);

	/* FIXME If (operation is completed) { */
	return lan9118_reg_read(dev, LAN9118_MAC_CSR_DATA);
}

static void lan9118_mac_write(struct net_device *dev, unsigned int offset, uint32_t val) {
	uint32_t l;

	l = lan9118_reg_read(dev, LAN9118_MAC_CSR_CMD);

	if (l & _LAN9118_MAC_CSR_CMD_CSR_BUSY) {
		printk("MAC is busy\n");
		return;
	}

	lan9118_reg_write(dev, LAN9118_MAC_CSR_DATA, val);

	lan9118_reg_write(dev, LAN9118_MAC_CSR_CMD, ((offset & 0xFF) |
			_LAN9118_MAC_CSR_CMD_CSR_BUSY));

	/* Workaround for hardware read-after-write restriction. */
	l = lan9118_reg_read(dev, LAN9118_BYTE_TEST);

	/* FIXME check if operation was completed successfully. */
}

#define UGLY_MASK (1 << 18)

void lan9118_irq_handler(pin_mask_t ch_mask, pin_mask_t mon_mask) {
	uint32_t l = lan9118_reg_read(nic, LAN9118_INT_STS);

	//if (l & _LAN9118_INT_STS_RXSTOP_INT_) {
		printk("lan9118: receive packet, enable - %X, status - %X\n",
			 lan9118_reg_read(nic, LAN9118_INT_EN), l);
	//}

	lan9118_reg_write(nic, LAN9118_INT_STS, l & ~UGLY_MASK);
}

static int lan9118_reset(struct net_device *dev) {
	unsigned int timeout = 10;
	unsigned int l;

    /* Software reset the LAN911x */
	lan9118_reg_write(dev, LAN9118_HW_CFG, _LAN9118_HW_CFG_SRST);

	do {
		usleep(10);
		l = lan9118_reg_read(dev, LAN9118_HW_CFG);
	} while (--timeout && (l & _LAN9118_HW_CFG_SRST));

	if (l & _LAN9118_HW_CFG_SRST) {
		return -EIO;
	}

	return 0;
}

static void lan9118_disable_irqs(struct net_device *dev) {
	/* Initialize interrupts */
	lan9118_reg_write(dev, LAN9118_INT_EN, 0);
	lan9118_reg_write(dev, LAN9118_INT_STS, 0xFFFFFFFF);
}

static int lan9118_open(struct net_device *dev) {
	uint32_t l;
	uint32_t irq_cfg;

	lan9118_reset(dev);

	lan9118_reg_write(dev, LAN9118_HW_CFG, 0x00050000);

	lan9118_disable_irqs(dev);

	irq_cfg = (1 << 24) | _LAN9118_IRQ_CFG_EN;
	irq_cfg |= _LAN9118_IRQ_CFG_POL | _LAN9118_IRQ_CFG_TYPE;
	lan9118_reg_write(dev, LAN9118_IRQ_CFG, irq_cfg);

	lan9118_reg_write(dev, LAN9118_GPIO_CFG, 0x70070000);


	l = lan9118_reg_read(dev, LAN9118_HW_CFG);
	/* Preserve TX FIFO size and external PHY configuration */
	l &= (_LAN9118_HW_CFG_TX_FIF_SZ_|0x00000FFF);
	l |= _LAN9118_HW_CFG_SF_;
	lan9118_reg_write(dev, LAN9118_HW_CFG, l);

	l = lan9118_reg_read(dev, LAN9118_FIFO_INT);
	l |= _LAN9118_FIFO_INT_TX_AVAIL_LEVEL_;
	l &= ~(_LAN9118_FIFO_INT_RX_STS_LEVEL_);
	lan9118_reg_write(dev, LAN9118_FIFO_INT, l);


	l = lan9118_reg_read(dev, LAN9118_INT_EN);
	l = _LAN9118_INT_EN_SW | _LAN9118_INT_EN_RSFL;
	printk("LAN9118_INT_EN: %X\n", l);
	lan9118_reg_write(dev, LAN9118_INT_EN, l);

	l = lan9118_mac_read(dev, LAN9118_MAC_CR);
	l |= (_LAN9118_MAC_CR_TXEN | _LAN9118_MAC_CR_RXEN | _LAN9118_MAC_CR_HBDIS);
	printk("LAN9118_MAC_CR: %X\n", l);
	lan9118_mac_write(dev, LAN9118_MAC_CR, l);

	/* GPIO */
	gpio_pin_irq_attach(LAN9118_PORT, LAN9118_PIN,
			lan9118_irq_handler, GPIO_MODE_INT_MODE_RISING, dev);

	return 0;
}

int lan9118_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver lan9118_drv_ops = {
	//.xmit = xmit,
	.start = lan9118_open,
	//.stop = stop,
	.set_macaddr = lan9118_set_macaddr
};

static int lan9118_init(void) {
	int res;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	printk("LAN9118 \n");

	nic->drv_ops = &lan9118_drv_ops;

	res = gpmc_cs_init(LAN9118_GPMC_CS, (uint32_t *) &nic->base_addr, SIZE_16M);

	if (res < 0)
		return -1;

	if (lan9118_reg_read(nic, LAN9118_BYTE_TEST) != 0x87654321) {
		printk("lan9118 bad BYTE_TEST register value - %d\n",
				lan9118_reg_read(nic, LAN9118_BYTE_TEST));
		return -1;
	}

	res = lan9118_reset(nic);

	if (res < 0) {
		printk("lan9118 reset failed\n");
		return res;
	}

	return inetdev_register_dev(nic);
}
