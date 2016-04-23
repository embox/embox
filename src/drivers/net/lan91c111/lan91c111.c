/**
 * @file
 *
 * @data 09.04.2016
 * @author: Anton Bondarev
 */
#include <errno.h>

#include <kernel/printk.h>
#include <kernel/irq.h>
#include <mem/misc/pool.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include <embox/unit.h>

/* Internal I/O space mapping */

#define BANK_BASE_ADDR 0xC8000000

/* Bank 0 */
#define BANK_TCR        (BANK_BASE_ADDR + 0x0)
#define BANK_EPH_STATUS (BANK_BASE_ADDR + 0x2)
#define BANK_RCR        (BANK_BASE_ADDR + 0x4)
#define BANK_COUNTER    (BANK_BASE_ADDR + 0x6)
#define BANK_MIR        (BANK_BASE_ADDR + 0x8)
#define BANK_RPCR       (BANK_BASE_ADDR + 0xA)
/*      BANK_RESERVED   (BANK_BASE_ADDR + 0xC) */
#define BANK_BANK       (BANK_BASE_ADDR + 0xE) /* Common for all banks */

/* Bank 1 */
#define BANK_CONFIG          (BANK_BASE_ADDR + 0x0)
#define BANK_BASE            (BANK_BASE_ADDR + 0x2)
#define BANK_IA01            (BANK_BASE_ADDR + 0x4)
#define BANK_IA23            (BANK_BASE_ADDR + 0x6)
#define BANK_IA45            (BANK_BASE_ADDR + 0x8)
#define BANK_GENERAL_PURPOSE (BANK_BASE_ADDR + 0xA)
#define BANK_CONTROL         (BANK_BASE_ADDR + 0xC)
/*      BANK_BANK            (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Bank 2 */
#define BANK_MMU_CMD    (BANK_BASE_ADDR + 0x0)
#define BANK_PNR        (BANK_BASE_ADDR + 0x2)
#define BANK_FIFO_PORTS (BANK_BASE_ADDR + 0x4)
#define BANK_POINTER    (BANK_BASE_ADDR + 0x6)
#define BANK_DATA       (BANK_BASE_ADDR + 0x8)
/*      BANK_DATA       (BANK_BASE_ADDR + 0xA) -- another 2 bytes */
#define BANK_INTERRUPT  (BANK_BASE_ADDR + 0xC)
/*      BANK_BANK       (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Bank 3 */
#define BANK_MT01     (BANK_BASE_ADDR + 0x0)
#define BANK_MT23     (BANK_BASE_ADDR + 0x2)
#define BANK_MT45     (BANK_BASE_ADDR + 0x4)
#define BANK_MT67     (BANK_BASE_ADDR + 0x6)
#define BANK_MGMT     (BANK_BASE_ADDR + 0x8)
#define BANK_REVISION (BANK_BASE_ADDR + 0xA)
#define BANK_RCV      (BANK_BASE_ADDR + 0xC)
/*      BANK_BANK     (BANK_BASE_ADDR + 0xE) -- already defined above */

#define REG16_LOAD(addr) \
	*((volatile uint16_t *)(addr))

#define REG16_STORE(addr, val) \
	do { *((volatile uint16_t *)(addr)) = (val); } while (0)

#define REG8_LOAD(addr) \
	*((volatile uint8_t *)(addr))

#define REG8_STORE(addr, val) \
	do { *((volatile uint8_t *)(addr)) = (val); } while (0)

/* Commands */
#define CMD_NOP                0
#define CMD_TX_ALLOC           1
#define CMD_RESET_MMU          2
#define CMD_RX_POP             3
#define CMD_RX_POP_AND_RELEASE 4
#define CMD_PACKET_FREE        5
#define CMD_TX_ENQUEUE         6
#define CMD_TX_FIFO_RESET      7

/* Various flags */
#define MMU_BUSY 0x0001
#define RX_EN    0x0100
#define TX_EN    0x0001

#define RX_INT   0x0100
#define TX_INT   0x0200
#define TX_ACK   0x0002

#define TX_EMPTY 0x80

#define TX_MASK  0x0002
#define RX_MASK  0x0001

#define CRC_CONTROL   0x10
#define ODD_CONTROL   0x20

#define LAN91C111_FRAME_SIZE_MAX 2048
#define LAN91C111_IRQ            27

#define PNUM_MASK 0x3F

struct lan91c111_frame {
	uint16_t status;
	uint16_t count; /* In bytes, 5 high bits are reserved */
	uint16_t data[LAN91C111_FRAME_SIZE_MAX];
};

/**
 * @brief Set active bank ID
 */
static void _set_bank(int n) {
	assert(0 <= n && n <= 3);
	REG16_STORE(BANK_BANK, (uint16_t) n);
}
#if 0
static void _regdump(void) {
	for (int i = 0; i <= 3; i++) {
		_set_bank(i);
		printk("Bank %d\n", i);
		for (int j = 0; j < 7; j++) {
			printk("%4x\n", (uint16_t) REG16_LOAD(BANK_BASE_ADDR + 2 * j));
		}
	}
}
#endif

/**
 * @brief Set approprate opcode
 */
static void _set_cmd(int opcode) {
	uint16_t val;
	assert(0 <= opcode && opcode <= 7);

	_set_bank(2);
	while (REG16_LOAD(BANK_MMU_CMD) & MMU_BUSY) {
		/* BUSY */
	}
	val = opcode << 5;
	REG16_STORE(BANK_MMU_CMD, val);
}

static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint16_t packet_num;
	int i;
	uint8_t *data;
	uint16_t pointer;

	_set_cmd(CMD_TX_ALLOC);

	_set_bank(2);

	packet_num = (REG16_LOAD(BANK_PNR) >> 8) & PNUM_MASK;

	REG16_STORE(BANK_PNR, packet_num << 8);

	pointer = 0;
	REG16_STORE(BANK_POINTER, pointer);

	/* Write header */
	pointer = 2;
	REG16_STORE(BANK_POINTER, pointer);
	REG16_STORE(BANK_DATA, 2 * (1 + 1 + 1 + 2 + (uint16_t) skb->len / 2));
	/* Those 10 bytes are 2 for status + 2 for counter +
	 * 4 for crc + 2 for control */

	pointer = 4;
	REG16_STORE(BANK_POINTER, pointer);

	/* BANK_DATA register works as FIFO, so we just push
	 * data with 16-bit writes */
	data = (uint8_t*) skb_data_cast_in(skb->data);
	for (i = 0; i < skb->len; i++) {
		/* This could be done with 32-bit writes,
		 * but here we just use the usual macro */
		REG8_STORE(BANK_DATA, *data);
		data++;

		/* Auto-increment for pointer register seems to
		 * be unsupported by qemu-linaro, so we increment
		 * it by hand */
		pointer++;
		REG16_STORE(BANK_POINTER, pointer);
	}

	for (int i = 0; i < 4; i++) {
		pointer++;
		REG16_STORE(BANK_POINTER, pointer);
		REG8_STORE(BANK_DATA, 0);
	}
	/* Miss CRC bytes */

	/* Write control byte */
	if (skb->len % 2) {
		pointer++;
		REG8_STORE(BANK_DATA, CRC_CONTROL | ODD_CONTROL);
	} else {
		pointer++;
		REG16_STORE(BANK_POINTER, pointer);
		REG8_STORE(BANK_DATA, 0x0);
		pointer++;
		REG8_STORE(BANK_POINTER, CRC_CONTROL);
	}

	_set_cmd(CMD_TX_ENQUEUE);

	skb_free(skb);
	return 0;
}

static int lan91c111_open(struct net_device *dev) {
        _set_bank(0);
	REG16_STORE(BANK_RCR, RX_EN);
	REG16_STORE(BANK_TCR, TX_EN);

	_set_bank(2);
	REG16_STORE(BANK_INTERRUPT, RX_INT | TX_INT);

	return 0;
}

static int lan91c111_set_macaddr(struct net_device *dev, const void *addr) {
        uint8_t *mac = (uint8_t*) addr;
	uint16_t mac_hi, mac_mid, mac_lo;

        assert(mac);

	mac_hi  = (mac[5] << 8) | (mac[4] << 0);
	mac_mid = (mac[3] << 8) | (mac[2] << 0);
	mac_lo  = (mac[1] << 8) | (mac[0] << 0);

        _set_bank(1);

        REG16_STORE(BANK_IA01, mac_lo);
        REG16_STORE(BANK_IA23, mac_mid);
        REG16_STORE(BANK_IA45, mac_hi);

	return 0;
}

static const struct net_driver lan91c111_drv_ops = {
	.xmit = lan91c111_xmit,
	.start = lan91c111_open,
	.set_macaddr = lan91c111_set_macaddr
};

static irq_return_t lan91c111_int_handler(unsigned int irq_num,
		void *dev_id) {
	struct sk_buff *skb;
	uint16_t buf;
	uint16_t len;
	uint16_t packet;
	uint8_t *skb_data;

	_set_bank(2);

	if (REG16_LOAD(BANK_INTERRUPT) & TX_MASK) {
		/* TX int */
		packet = REG16_LOAD(BANK_FIFO_PORTS) & 0xFF;
		if (!(packet & TX_EMPTY)) {
			REG16_STORE(BANK_PNR, packet & PNUM_MASK);
			_set_cmd(CMD_PACKET_FREE);
		}

		REG16_STORE(BANK_INTERRUPT, RX_INT | TX_INT | TX_ACK);
		return 0;
	}

	if (!(REG16_LOAD(BANK_INTERRUPT) & RX_MASK))
		return 0;

	REG16_STORE(BANK_POINTER, 2);
	len = (REG16_LOAD(BANK_DATA) & 0x7FF) - 10;
	/* In original structure, byte count includes headers, so
	 * we shrink it to data size */

	skb = skb_alloc(len);
	assert(skb);
	skb->len = len;
	skb->dev = dev_id;

	skb_data = skb_data_cast_in(skb->data);
	assert(skb_data);

	for (int i = 0; i < skb->len / 2 + skb->len % 2; i++) {
		REG16_STORE(BANK_POINTER, 0x8000 + 4 + i * 2);
		buf = REG16_LOAD(BANK_DATA);
		skb_data[i * 2] = buf & 0xFF;
		skb_data[i * 2 + 1] = buf >> 8;
		/* TODO buffer overflow if len is odd */
	}

	netif_rx(skb);

	_set_cmd(CMD_RX_POP_AND_RELEASE);
	return 0;
}

EMBOX_UNIT_INIT(lan91c111_init);
static int lan91c111_init(void) {
        struct net_device *nic;

        if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

	irq_attach(LAN91C111_IRQ, lan91c111_int_handler, 0, nic, "lan91c111");

        nic->drv_ops = &lan91c111_drv_ops;

	return inetdev_register_dev(nic);
}
