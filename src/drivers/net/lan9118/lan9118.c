/**
 * @file
 * @brief Ethernet Controller SMSC LAN9118
 *
 * @date 30.10.13
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <hal/pins.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <util/log.h>

#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

#define LAN9118_IRQ_GPIO          0
#define LAN9118_IRQ_NO_GPIO       1
#define LAN9118_IRQ_TYPE          OPTION_GET(NUMBER, irq_type)

#if LAN9118_IRQ_TYPE == LAN9118_IRQ_GPIO /* If GPIO irq */
#include <drivers/gpmc.h>
#include <drivers/gpio/gpio.h>

#define LAN9118_GPMC_CS           5 /* GPMC chip-select number */
#define LAN9118_PORT              (OPTION_GET(NUMBER, port) - 1)
#define LAN9118_PIN               OPTION_GET(NUMBER, irq_pin)
#define LAN9118_MEMORY_REG_SIZE   OPTION_GET(NUMBER, memory_region_size)

#elif LAN9118_IRQ_TYPE == LAN9118_IRQ_NO_GPIO /* If interrupt controller ("usual" irq) */
#include <drivers/common/memory.h> // TODO: move out of here when mmu will be enable on arm/overo

#define LAN9118_BASE_ADDRESS      OPTION_GET(NUMBER, base_address)
#define LAN9118_MEMORY_REG_SIZE   OPTION_GET(NUMBER, memory_region_size)
#define LAN9118_IRQ_NR            OPTION_GET(NUMBER, irq_nr)

#else
#error LAN9118: not supported IRQ type. Please select from usual one and GPIO irq.
#endif

#define LAN9118_RX_DATA_FIFO   0x00

#define LAN9118_TX_DATA_FIFO   0x20
#define _LAN9118_TX_CMD_A_FIRST_SEG_       0x00002000
#define _LAN9118_TX_CMD_A_LAST_SEG_        0x00001000

#define LAN9118_RX_STATUS_FIFO 0x40
#define _LAN9118_RX_STS_ES                 0x00008000
#define _LAN9118_RX_STS_LE                 0x00001000
#define _LAN9118_RX_STS_ME                 0x00000008
#define _LAN9118_RX_STS_CRC                0x00000002

/* CONTROL AND STATUS REGISTERS */
#define LAN9118_ID_REV         0x50

#define LAN9118_IRQ_CFG        0x54
#define _LAN9118_IRQ_CFG_EN                0x00000100
#define _LAN9118_IRQ_CFG_POL               0x00000010
#define _LAN9118_IRQ_CFG_TYPE              0x00000001

#define LAN9118_INT_STS        0x58
#define _LAN9118_INT_STS_RXSTOP_INT        0x01000000
#define _LAN9118_INT_STS_RSFL_INT          0x00000008

#define LAN9118_INT_EN         0x5C
#define _LAN9118_INT_EN_SW                 0x80000000
#define _LAN9118_INT_EN_RSFL               0x00000008
#define _LAN9118_INT_EN_RXSTOP_INT         0x01000000

#define LAN9118_BYTE_TEST      0x64

#define LAN9118_FIFO_INT       0x68
#define _LAN9118_FIFO_INT_TX_AVAIL_LEVEL   0xFF000000
#define _LAN9118_FIFO_INT_TX_STS_LEVEL_    0x00FF0000
#define _LAN9118_FIFO_INT_RX_AVAIL_LEVEL   0x0000FF00
#define _LAN9118_FIFO_INT_RX_STS_LEVEL     0x000000FF

#define LAN9118_RX_CFG         0x6C

#define LAN9118_TX_CFG         0x70
#define _LAN9118_TX_CFG_TX_ON              0x00000002
#define _LAN9118_TX_CFG_STOP_TX            0x00000001

#define LAN9118_HW_CFG         0x74
#define _LAN9118_HW_CFG_MBO_               0x00100000
#define _LAN9118_HW_CFG_TX_FIF_SZ_         0x000F0000
#define _LAN9118_HW_CFG_SRST               0x00000001

#define LAN9118_RX_DP_CTL      0x78

#define LAN9118_RX_FIFO_INF    0x7C
#define _LAN9118_RX_FIFO_INF_RXSUSED       0x00FF0000
#define _LAN9118_RX_FIFO_INF_RXDUSED       0x0000FFFF

#define LAN9118_TX_FIFO_INF    0x80
#define _LAN9118_TX_FIFO_INF_TDFREE        0x0000FFFF

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
#define LAN9118_MAC_ADDRH      0x02
#define LAN9118_MAC_ADDRL      0x03
#define _LAN9118_MAC_CR_HBDIS              0x10000000
#define _LAN9118_MAC_CR_TXEN               0x00000008
#define _LAN9118_MAC_CR_RXEN               0x00000004

EMBOX_UNIT_INIT(lan9118_init);

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
		log_info("MAC is busy");
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
		log_info("MAC is busy");
		return;
	}

	lan9118_reg_write(dev, LAN9118_MAC_CSR_DATA, val);

	lan9118_reg_write(dev, LAN9118_MAC_CSR_CMD, ((offset & 0xFF) |
			_LAN9118_MAC_CSR_CMD_CSR_BUSY));

	/* Workaround for hardware read-after-write restriction. */
	l = lan9118_reg_read(dev, LAN9118_BYTE_TEST);

	/* FIXME check if operation was completed successfully. */
}

static int lan9118_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint32_t l, freespace;
	uint32_t *src, *dst;
	int wrsz;

	freespace = lan9118_reg_read(dev, LAN9118_TX_FIFO_INF) & _LAN9118_TX_FIFO_INF_TDFREE;

	log_debug("skb_len - %d, freespace - %d", skb->len, freespace);

	irq_lock();
	{
		l = (uint32_t) ((unsigned long)skb->mac.raw & 0x03) << 16;
		l |= _LAN9118_TX_CMD_A_FIRST_SEG_ | _LAN9118_TX_CMD_A_LAST_SEG_;
		l |= skb->len;
		lan9118_reg_write(dev, LAN9118_TX_DATA_FIFO, l);

		l = skb->len << 16;
		l |= skb->len;
		lan9118_reg_write(dev, LAN9118_TX_DATA_FIFO, l);

		src = (uint32_t*) ((uint32_t)skb->mac.raw & (~0x3));
		dst = (uint32_t*) (dev->base_addr + LAN9118_TX_DATA_FIFO);

		assert(freespace >= skb->len);

		wrsz = (uint32_t)skb->len + 3;
		wrsz += (uint32_t)((unsigned long)skb->mac.raw & 0x3);
		wrsz >>= 2;

		while (wrsz > 0) {
			*dst = *src++;
			wrsz--;
		}

		skb_free(skb);
	}
	irq_unlock();

	return 0;
}

static uint32_t lan9118_rx_status(struct net_device *dev) {
	uint32_t rx_status;

	rx_status = lan9118_reg_read(dev, LAN9118_RX_FIFO_INF) & _LAN9118_RX_FIFO_INF_RXSUSED;

	if (rx_status != 0)
		rx_status = lan9118_reg_read(dev, LAN9118_RX_STATUS_FIFO);

	return rx_status;
}

static void lan9118_rx(struct net_device *dev) {
	uint32_t rx_status;
	int packet_len;
	struct sk_buff *skb;
	uint32_t *dst;

	irq_lock();
	{
		rx_status = lan9118_rx_status(dev);
		assert(rx_status != 0);

		packet_len = ((rx_status & 0x3FFF0000) >> 16);
repeat:
		packet_len -= 4; /* checksum */

		if (rx_status & _LAN9118_RX_STS_ES) {
			log_debug("error rx_status - 0x%08x", rx_status);
			irq_unlock();
			return;
		}

		if ((skb = skb_alloc(packet_len))) {
			log_debug("packet_len - %d", packet_len);

			dst = (uint32_t*) skb->mac.raw;

			while (packet_len > 0) {
				*dst++ = lan9118_reg_read(dev, LAN9118_RX_DATA_FIFO);
				packet_len -= 4;
			}
			/* and read the last word */
			lan9118_reg_read(dev, LAN9118_RX_DATA_FIFO);

			skb->dev = dev;
			//dev->stats.rx_packets++;
			//dev->stats.rx_bytes += skb->len;
			netif_rx(skb);
		} else {
			//dev->stats.rx_dropped++;
			log_debug("dropped %d", packet_len);
		}

		rx_status = lan9118_rx_status(dev);
		packet_len = ((rx_status & 0x3FFF0000) >> 16);

		if (packet_len > 0)
			goto repeat;

		assertf(packet_len == 0, "packet_len - %d\n", packet_len);
	}
	irq_unlock();
}

irq_return_t lan9118_irq_handler(unsigned int irq_nr, void *nic) {
	uint32_t l = lan9118_reg_read(nic, LAN9118_INT_STS);

	if (l & _LAN9118_INT_STS_RSFL_INT) {
		lan9118_rx(nic);
	}

	lan9118_reg_write(nic, LAN9118_INT_STS, l);

	return IRQ_HANDLED;
}

void lan9118_gpio_irq_handler(void *nic) {
	uint32_t l = lan9118_reg_read(nic, LAN9118_INT_STS);

	if (l & _LAN9118_INT_STS_RSFL_INT) {
		lan9118_rx(nic);
	}

	lan9118_reg_write(nic, LAN9118_INT_STS, l);
}

static void mdelay(int value) {
	volatile int delay = value;
	while (delay --);
}

static int lan9118_reset(struct net_device *dev) {
	unsigned int timeout = 10;
	unsigned int l;

	/* Software reset the LAN911x */
	lan9118_reg_write(dev, LAN9118_HW_CFG, _LAN9118_HW_CFG_SRST);

	do {
		mdelay(1000);
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
	int res;

	res = lan9118_reset(dev);
	if (res < 0) {
		return res;
	}

	lan9118_disable_irqs(dev);

	lan9118_reg_write(dev, LAN9118_IRQ_CFG,
			(1 << 24) |
			_LAN9118_IRQ_CFG_EN |
			_LAN9118_IRQ_CFG_POL |
			_LAN9118_IRQ_CFG_TYPE);

#if LAN9118_IRQ_TYPE == LAN9118_IRQ_GPIO
	gpio_setup_mode(LAN9118_PORT, 1 << LAN9118_PIN,
		GPIO_MODE_INT_MODE_LEVEL1 |
		GPIO_MODE_IN |
		GPIO_MODE_IN_INT_EN);

	gpio_irq_attach(LAN9118_PORT, 1 << LAN9118_PIN,
				lan9118_gpio_irq_handler,
				dev);
#else
	irq_attach(LAN9118_IRQ_NR, lan9118_irq_handler, 0, dev, "lan9118 INTERRUPT");
#endif

	lan9118_reg_write(dev, LAN9118_HW_CFG, 0x00050000);

	lan9118_reg_write(dev, LAN9118_GPIO_CFG, 0x70070000);

	l = lan9118_reg_read(dev, LAN9118_HW_CFG);
	lan9118_reg_write(dev, LAN9118_HW_CFG, l | _LAN9118_HW_CFG_MBO_);

	lan9118_reg_write(dev, LAN9118_INT_EN, (_LAN9118_INT_EN_SW | _LAN9118_INT_EN_RSFL));

	l = lan9118_mac_read(dev, LAN9118_MAC_CR);
	l |= (_LAN9118_MAC_CR_TXEN | _LAN9118_MAC_CR_RXEN | _LAN9118_MAC_CR_HBDIS);
	log_debug("LAN9118_MAC_CR: %X", l);
	lan9118_mac_write(dev, LAN9118_MAC_CR, l);

	lan9118_reg_write(dev, LAN9118_TX_CFG, _LAN9118_TX_CFG_TX_ON);

	return 0;
}

static void lan9119_get_macaddr(struct net_device *dev, void *addr) {
	*(uint32_t *) addr       = lan9118_mac_read(dev, LAN9118_MAC_ADDRL);
	*(uint16_t *) (addr + 4) = lan9118_mac_read(dev, LAN9118_MAC_ADDRH);
}

int lan9118_set_macaddr(struct net_device *dev, const void *addr) {
	uint32_t tmp;
	char data[6];
	char *p_tmp = (char *)addr;

	lan9118_disable_irqs(dev);

	lan9119_get_macaddr(dev, data);
	log_debug("Before set_macaddr: %x:%x:%x:%x:%x:%x\n", data[0], data[1], data[2],
		data[3], data[4], data[5]);
	data[0] = p_tmp[0];
	data[1] = p_tmp[1];
	data[2] = p_tmp[2];
	data[3] = p_tmp[3];

	memcpy(&tmp, &data[0], 4);
	lan9118_mac_write(dev, LAN9118_MAC_ADDRL, tmp);

	tmp = 0;
	data[0] = p_tmp[4];
	data[1] = p_tmp[5];
	data[2] = 0;
	data[3] = 0;
	memcpy(&tmp, &data[0], 2);

	lan9118_mac_write(dev, LAN9118_MAC_ADDRH, tmp);

	lan9119_get_macaddr(dev, data);
	log_debug("After set_macaddr: %x:%x:%x:%x:%x:%x\n", data[0], data[1], data[2],
		data[3], data[4], data[5]);

	lan9118_reg_write(dev, LAN9118_INT_EN, (_LAN9118_INT_EN_SW | _LAN9118_INT_EN_RSFL));
	return 0;
}

static const struct net_driver lan9118_drv_ops = {
	.xmit = lan9118_xmit,
	.start = lan9118_open,
	//.stop = stop,
	.set_macaddr = lan9118_set_macaddr
};

static int lan9118_init(void) {
	int res = 0;
	struct net_device *nic;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &lan9118_drv_ops;

#if LAN9118_IRQ_TYPE == LAN9118_IRQ_GPIO
	res = gpmc_cs_init(LAN9118_GPMC_CS, (uint32_t *) &nic->base_addr, LAN9118_MEMORY_REG_SIZE);
#else
	nic->base_addr = LAN9118_BASE_ADDRESS;
#endif

	if (res < 0)
		return -1;

	if (lan9118_reg_read(nic, LAN9118_BYTE_TEST) != 0x87654321) {
		log_info("lan9118 bad BYTE_TEST register value - %d",
				lan9118_reg_read(nic, LAN9118_BYTE_TEST));
		return -1;
	}

	res = lan9118_reset(nic);

	if (res < 0) {
		log_info("lan9118 reset failed");
		return res;
	}

	return inetdev_register_dev(nic);
}

#if LAN9118_IRQ_TYPE != LAN9118_IRQ_GPIO
/* Static memory (CS3) Ethernet 0x4E000000-0x4EFFFFFF (SMC) 16MB */
PERIPH_MEMORY_DEFINE(lan9118, LAN9118_BASE_ADDRESS, LAN9118_MEMORY_REG_SIZE);

#endif
