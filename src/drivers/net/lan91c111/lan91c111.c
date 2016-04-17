/**
 * @file
 *
 * @data 09.04.2016
 * @author: Anton Bondarev
 */
#include <errno.h>

#include <kernel/irq.h>
#include <mem/misc/pool.h>

#include <net/skbuff.h>
#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/l2/ethernet.h>

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

/* Commands */
#define CMD_NOP                0
#define CMD_TX_ALLOC           1
#define CMD_RESET_MMU          2
#define CMD_RX_POP             3
#define CMD_RX_POP_AND_RELEASE 4
#define CMD_PACKET_FREE        5
#define CMD_TX_ENQUEUE         6
#define CMD_TX_FIFO_RESET      7

#define LAN91C111_FRAME_SIZE_MAX 2048

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

/**
 * @brief Set approprate opcode
 */
static void _set_cmd(int opcode) {
	uint16_t val;
	assert(0 <= opcode && opcode <= 7);

	_set_bank(2);
	while (REG16_LOAD(BANK_MMU_CMD) & 0x1) {
		/* BUSY */
	}
	val = opcode << 5;
	REG16_STORE(BANK_MMU_CMD, val);
}

static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint16_t packet_num;
	int i;
	uint16_t *data;

	_set_cmd(CMD_TX_ALLOC);

	_set_bank(2);
	while (!(REG16_LOAD(BANK_INTERRUPT) & (1 << 3))){
	};

	packet_num = (REG16_LOAD(BANK_PNR) >> 8) & 0x3F;

	REG16_STORE(BANK_PNR, packet_num << 8);

	/* TODO write pointer register. WTF it's 8 bit */

	data = (uint16_t*) skb_data_cast_in(skb->data);
	for (i = 0; i < skb->len; i += 2) {
		/* This could be done with 32-bit writes,
		 * but here we just use the usual macro */
		REG16_STORE(BANK_DATA, *data);
		data++;
	}

	_set_cmd(CMD_TX_ENQUEUE);

	/* TODO handle service interrupt */

	skb_free(skb);
	return 0;
}

static int lan91c111_open(struct net_device *dev) {
        _set_bank(0);
	REG16_STORE(BANK_RCR, 0x0100); /* Enable RX interrupts */
	return 0;
}

static int lan91c111_set_macaddr(struct net_device *dev, const void *addr) {
        uint8_t *mac = (uint8_t*) addr;
	uint16_t mac_hi, mac_mid, mac_lo;

        assert(mac);

	mac_hi  = (mac[4] << 8) | (mac[5] << 0);
	mac_mid = (mac[2] << 8) | (mac[3] << 0);
	mac_lo  = (mac[0] << 8) | (mac[1] << 0);

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

EMBOX_UNIT_INIT(lan91c111_init);
static int lan91c111_init(void) {
        struct net_device *nic;

        if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

        nic->drv_ops = &lan91c111_drv_ops;

        /* TODO add rx interrupt handle */

	return inetdev_register_dev(nic);
}
