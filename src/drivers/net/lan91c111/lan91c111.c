/**
 * @file
 *
 * @data 09.04.2016
 * @author: Anton Bondarev
 */
#include <kernel/irq.h>
#include <mem/misc/pool.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <embox/unit.h>

/* Internal I/O space mapping */

#define BANK_BASE_ADDR 0x00000000 /* TODO */

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
#define BANK_IA35            (BANK_BASE_ADDR + 0x8)
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

#define REG16_LOAD(addr, val) \
	*((volatile uint16_t *)(addr))

#define REG16_STORE(addr, val) \
	do { *((volatile uint16_t *)(addr)) = (val); } while (0)

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
 * @brief Safe allocation from pool
 */
static struct lan91c111_frame *_frame_alloc(void) {
	struct lan91c111_frame *frame;

	frame = NULL;

	return frame;
}

static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct lan91c111_frame *tx_frame;

	tx_frame = _frame_alloc();
	if (tx_frame) {
	}

	skb_free(skb);
	return 0;
}

static int lan91c111_open(struct net_device *dev) {
	return 0;
}

static int lan91c111_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver lan91c111_drv_ops = {
	.xmit = lan91c111_xmit,
	.start = lan91c111_open,
	.set_macaddr = lan91c111_set_macaddr
};

EMBOX_UNIT_INIT(lan91c111_init);
static int lan91c111_init(void) {
	_set_bank(0); /* Just to use it */

	return 0;
}
