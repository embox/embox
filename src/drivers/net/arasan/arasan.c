/**
 * @file arasan.c
 * @brief Arasan ethernet driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 27.12.2016
 */

#include <errno.h>
#include <sys/mman.h>

#include <util/log.h>

#include <drivers/common/memory.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <mem/misc/pool.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>
#include <util/log.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

/* Internal I/O space mapping */
#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)
#define ARASAN_IRQ OPTION_GET(NUMBER, irq_num)

#define DMA_CONFIGURATION                         (BASE_ADDR + 0x0000)
#define DMA_CONTROL                               (BASE_ADDR + 0x0004)
#define DMA_STATUS_AND_IRQ                        (BASE_ADDR + 0x0008)
#define DMA_INTERRUPT_ENABLE                      (BASE_ADDR + 0x000C)
#define DMA_TRANSMIT_AUTO_POLL_COUNTER            (BASE_ADDR + 0x0010)
#define DMA_TRANSMIT_POLL_DEMAND                  (BASE_ADDR + 0x0014)
#define DMA_RECEIVE_POLL_DEMAND                   (BASE_ADDR + 0x0018)
#define DMA_TRANSMIT_BASE_ADDRESS                 (BASE_ADDR + 0x001C)
#define DMA_RECEIVE_BASE_ADDRESS                  (BASE_ADDR + 0x0020)
#define DMA_MISSED_FRAME_COUNTER                  (BASE_ADDR + 0x0024)
#define DMA_STOP_FLUSH_COUNTER                    (BASE_ADDR + 0x0028)
#define DMA_RECEIVE_INTERRUPT_MITIGATION          (BASE_ADDR + 0x002C)
#define DMA_CURRENT_TRANSMIT_DESCRIPTOR_POINTER   (BASE_ADDR + 0x0030)
#define DMA_CURRENT_TRANSMIT_BUFFER_POINTER       (BASE_ADDR + 0x0034)
#define DMA_CURRENT_RECEIVE_DESCRIPTOR_POINTER    (BASE_ADDR + 0x0038)
#define DMA_CURRENT_RECEIVE_BUFFER_POINTER        (BASE_ADDR + 0x003C)

#define MAC_GLOBAL_CONTROL                        (BASE_ADDR + 0x0100)
#define MAC_TRANSMIT_CONTROL                      (BASE_ADDR + 0x0104)
#define MAC_RECEIVE_CONTROL                       (BASE_ADDR + 0x0108)
#define MAC_MAXIMUM_FRAME_SIZE                    (BASE_ADDR + 0x010C)
#define MAC_TRANSMIT_JABBER_SIZE                  (BASE_ADDR + 0x0110)
#define MAC_RECEIVE_JABBER_SIZE                   (BASE_ADDR + 0x0114)
#define MAC_ADDRESS_CONTROL                       (BASE_ADDR + 0x0118)
#define MAC_MDIO_CLOCK_DIVISION_CONTROL           (BASE_ADDR + 0x011C)
#define MAC_ADDRESS1_HIGH                         (BASE_ADDR + 0x0120)
#define MAC_ADDRESS1_MED                          (BASE_ADDR + 0x0124)
#define MAC_ADDRESS1_LOW                          (BASE_ADDR + 0x0128)
#define MAC_ADDRESS2_HIGH                         (BASE_ADDR + 0x012C)
#define MAC_ADDRESS2_MED                          (BASE_ADDR + 0x0130)
#define MAC_ADDRESS2_LOW                          (BASE_ADDR + 0x0134)
#define MAC_ADDRESS3_HIGH                         (BASE_ADDR + 0x0138)
#define MAC_ADDRESS3_MED                          (BASE_ADDR + 0x013C)
#define MAC_ADDRESS3_LOW                          (BASE_ADDR + 0x0140)
#define MAC_ADDRESS4_HIGH                         (BASE_ADDR + 0x0144)
#define MAC_ADDRESS4_MED                          (BASE_ADDR + 0x0148)
#define MAC_ADDRESS4_LOW                          (BASE_ADDR + 0x014C)
#define MAC_HASH_TABLE1                           (BASE_ADDR + 0x0150)
#define MAC_HASH_TABLE2                           (BASE_ADDR + 0x0154)
#define MAC_HASH_TABLE3                           (BASE_ADDR + 0x0158)
#define MAC_HASH_TABLE4                           (BASE_ADDR + 0x015C)

#define MAC_MDIO_CONTROL                          (BASE_ADDR + 0x01A0)
#define MAC_MDIO_DATA                             (BASE_ADDR + 0x01A4)
#define MAC_RX_STATCTR_CONTROL                    (BASE_ADDR + 0x01A8)
#define MAC_RX_STATCTR_DATA_HIGH                  (BASE_ADDR + 0x01AC)
#define MAC_RX_STATCTR_DATA_LOW                   (BASE_ADDR + 0x01B0)
#define MAC_TX_STATCTR_CONTROL                    (BASE_ADDR + 0x01B4)
#define MAC_TX_STATCTR_DATA_HIGH                  (BASE_ADDR + 0x01B8)
#define MAC_TX_STATCTR_DATA_LOW                   (BASE_ADDR + 0x01BC)
#define MAC_TRANSMIT_FIFO_ALMOST_FULL             (BASE_ADDR + 0x01C0)
#define MAC_TRANSMIT_PACKET_START_THRESHOLD       (BASE_ADDR + 0x01C4)
#define MAC_RECEIVE_PACKET_START_THRESHOLD        (BASE_ADDR + 0x01C8)
#define MAC_TRANSMIT_FIFO_ALMOST_EMPTY_THRESHOLD  (BASE_ADDR + 0x01CC)
#define MAC_INTERRUPT                             (BASE_ADDR + 0x01E0)
#define MAC_INTERRUPT_ENABLE                      (BASE_ADDR + 0x01E4)
#define MAC_VLAN_TPID1                            (BASE_ADDR + 0x01E8)
#define MAC_VLAN_TPID2                            (BASE_ADDR + 0x01EC)
#define MAC_VLAN_TPID3                            (BASE_ADDR + 0x01F0)

#define MAC_RECEIVE_CONTROL_RECEIVE_ENABLE            (1 << 0)
#define MAC_RECEIVE_CONTROL_STORE_AND_FORWARD         (1 << 3)

#define DMA_CONFIGURATION_SOFT_RESET                  (1 << 0)
#define DMA_CONFIGURATION_WAIT_FOR_DONE               (1 << 16)

#define DMA_CONTROL_START_TRANSMIT_DMA                (1 << 0)
#define DMA_CONTROL_START_RECEIVE_DMA                 (1 << 1)

#define DMA_STATUS_AND_IRQ_TRANSFER_DONE              (1 << 0)
#define DMA_STATUS_AND_IRQ_TRANS_DESC_UNVAIL          (1 << 1)
#define DMA_STATUS_AND_IRQ_TX_DMA_STOPPED             (1 << 2)
#define DMA_STATUS_AND_IRQ_RECEIVE_DONE               (1 << 4)
#define DMA_STATUS_AND_IRQ_RX_DMA_STOPPED             (1 << 6)
#define DMA_STATUS_AND_IRQ_MAC_INTERRUPT              (1 << 11)

#define DMA_INTERRUPT_ENABLE_TRANSMIT_DONE            (1 << 0)
#define DMA_INTERRUPT_ENABLE_RECEIVE_DONE             (1 << 4)
#define MAC_TRANSMIT_CONTROL_TRANSMIT_ENABLE          (1 << 0)


#define MAC_MDIO_CONTROL_READ_WRITE(VAL)              ((VAL) << 10)
#define MAC_MDIO_CONTROL_REG_ADDR(VAL)                ((VAL) << 5)
#define MAC_MDIO_CONTROL_PHY_ADDR(VAL)                ((VAL) << 0)
#define MAC_MDIO_CONTROL_START_FRAME(VAL)             ((VAL) << 15)


/* DMA descriptor fields */
#define DMA_RDES0_OWN_BIT      (1 << 31)
#define DMA_RDES0_FD           (1 << 30)
#define DMA_RDES0_LD           (1 << 29)

#define DMA_RDES1_EOR          (1 << 26)

#define DMA_TDES0_OWN_BIT      (1 << 31)
#define DMA_TDES1_IOC          (1 << 31)
#define DMA_TDES1_LS           (1 << 30)
#define DMA_TDES1_FS           (1 << 29)
#define DMA_TDES1_EOR          (1 << 26)

extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);

static void _reg_dump() {
	dcache_flush((void*)BASE_ADDR, 0x200);
	dcache_inval((void*)BASE_ADDR, 0x200);
	printk("Arasan ethernet DMA registers:\n");
	printk("======================================================\n");
	printk("CONFIGURATION                       %8x\n", REG32_LOAD(DMA_CONFIGURATION));
	printk("CONTROL                             %8x\n", REG32_LOAD(DMA_CONTROL));
	printk("STATUS_AND_IRQ                      %8x\n", REG32_LOAD(DMA_STATUS_AND_IRQ));
	printk("INTERRUPT_ENABLE                    %8x\n", REG32_LOAD(DMA_INTERRUPT_ENABLE));
	printk("TRANSMIT_AUTO_POLL_COUNTER          %8x\n", REG32_LOAD(DMA_TRANSMIT_AUTO_POLL_COUNTER));
	printk("TRANSMIT_POLL_DEMAND                %8x\n", REG32_LOAD(DMA_TRANSMIT_POLL_DEMAND));
	printk("RECEIVE_POLL_DEMAND                 %8x\n", REG32_LOAD(DMA_RECEIVE_POLL_DEMAND));
	printk("TRANSMIT_BASE_ADDRESS               %8x\n", REG32_LOAD(DMA_TRANSMIT_BASE_ADDRESS));
	printk("RECEIVE_BASE_ADDRESS                %8x\n", REG32_LOAD(DMA_RECEIVE_BASE_ADDRESS));
	printk("MISSED_FRAME_COUNTER                %8x\n", REG32_LOAD(DMA_MISSED_FRAME_COUNTER));
	printk("STOP_FLUSH_COUNTER                  %8x\n", REG32_LOAD(DMA_STOP_FLUSH_COUNTER));
	printk("RECEIVE_INTERRUPT_MITIGATION        %8x\n", REG32_LOAD(DMA_RECEIVE_INTERRUPT_MITIGATION));
	printk("CURRENT_TRANSMIT_DESCRIPTOR_POINTER %8x\n", REG32_LOAD(DMA_CURRENT_TRANSMIT_DESCRIPTOR_POINTER));
	printk("CURRENT_TRANSMIT_BUFFER_POINTER     %8x\n", REG32_LOAD(DMA_CURRENT_TRANSMIT_BUFFER_POINTER));
	printk("CURRENT_RECEIVE_DESCRIPTOR_POINTER  %8x\n", REG32_LOAD(DMA_CURRENT_RECEIVE_DESCRIPTOR_POINTER));
	printk("CURRENT_RECEIVE_BUFFER_POINTER      %8x\n", REG32_LOAD(DMA_CURRENT_RECEIVE_BUFFER_POINTER));
	printk("======================================================\n");
}

struct arasan_dma_desc {
	uint32_t status;
	uint32_t misc;
	uint32_t buffer1;
	uint32_t buffer2;
};

#define TX_RING_SIZE 8
#define RX_RING_SIZE 128

static struct arasan_dma_desc _rx_ring[RX_RING_SIZE];
static struct arasan_dma_desc _tx_ring[TX_RING_SIZE];
static int _tx_head, _tx_tail;
static int _rx_head, _rx_tail;

static int arasan_xmit(struct net_device *dev, struct sk_buff *skb) {
	printk("Arasan xmit\n");
	struct arasan_dma_desc *d;

	_tx_tail = (_tx_tail + 1) % TX_RING_SIZE;

	d = &_tx_ring[_tx_tail];

	d->buffer1 = (uint32_t) skb_get_data_pointner(skb->data);
	d->misc    = (DMA_TDES1_LS | DMA_TDES1_FS | (skb->len & 0xFFF));
	d->status  = DMA_TDES0_OWN_BIT | DMA_TDES1_IOC;

	REG32_STORE(DMA_TRANSMIT_POLL_DEMAND, 1);

	_reg_dump();
	return 0;
}

static int arasan_open(struct net_device *dev) {
	return 0;
}

static int arasan_set_macaddr(struct net_device *dev, const void *addr) {
	uint32_t mac_hi, mac_mid, mac_lo;
	const uint8_t *_macaddr = addr;

	log_debug("addr = %x:%x:%x:%x:%x:%x",
		_macaddr[0], _macaddr[1], _macaddr[2],
		_macaddr[3], _macaddr[4], _macaddr[5]);

	mac_lo  = (_macaddr[5] << 8) |
	          (_macaddr[4] << 0);

	mac_mid = (_macaddr[3] << 8) |
	          (_macaddr[2] << 0);

	mac_hi  = (_macaddr[1] << 8) |
	          (_macaddr[0] << 0);

	REG32_STORE(MAC_ADDRESS1_HIGH, mac_hi);
	REG32_STORE(MAC_ADDRESS1_MED, mac_mid);
	REG32_STORE(MAC_ADDRESS1_LOW, mac_lo);

	return 0;
}

static const struct net_driver arasan_drv_ops = {
	.xmit = arasan_xmit,
	.start = arasan_open,
	.set_macaddr = arasan_set_macaddr
};

static irq_return_t arasan_int_handler(unsigned int irq_num, void *dev_id) {

	return 0;
}

static int arasan_gemac_mdio_read(int mii_id, int regnum) {
	int value;

	REG32_STORE(MAC_MDIO_CONTROL,
			MAC_MDIO_CONTROL_READ_WRITE(1) |
			MAC_MDIO_CONTROL_REG_ADDR(regnum) |
			MAC_MDIO_CONTROL_PHY_ADDR(mii_id) |
			MAC_MDIO_CONTROL_START_FRAME(1));

	/* wait for end of transfer */
	while ((REG32_LOAD(MAC_MDIO_CONTROL) >> 15)) {
	}


	value = REG32_LOAD(MAC_MDIO_DATA);

	return value;
}

int arasan_gemac_mdio_write(int mii_id, int regnum, uint16_t value)
{
	REG32_STORE(MAC_MDIO_DATA, value);

	REG32_STORE(MAC_MDIO_CONTROL,
			MAC_MDIO_CONTROL_START_FRAME(1) |
			MAC_MDIO_CONTROL_PHY_ADDR(mii_id) |
			MAC_MDIO_CONTROL_REG_ADDR(regnum) |
			MAC_MDIO_CONTROL_READ_WRITE(0));

	/* wait for end of transfer */
	while ((REG32_LOAD(MAC_MDIO_CONTROL) >> 15)) {
	}

	return 0;
}
#define MII_PHYSID1     0x2 /* PHY ID #1 */
#define MII_PHYSID2     0x3 /* PHY ID #2 */
static int phy_discovery(void) {
	int id;
	int bus;
	for (bus = 0; bus < 32; bus ++) {
		id = arasan_gemac_mdio_read(bus, MII_PHYSID1) & 0xFFFF;
		if (id != 0xFFFF && id != 0x0 ) {
			return bus;
		}
	}
	return -1;
}

EMBOX_UNIT_INIT(arasan_init);
static int arasan_init(void) {
	struct net_device *nic;
	uint32_t reg;
	int mii_id;

	/* Setup TX descriptors */
	memset(_tx_ring, 0, TX_RING_SIZE * sizeof(struct arasan_dma_desc));
	_tx_head = _tx_tail = 0;
	REG32_STORE(DMA_TRANSMIT_BASE_ADDRESS, (uint32_t)_tx_ring);

	/* Setup RX descriptors */
	for (int i = 0; i < RX_RING_SIZE; i++) {
		_rx_ring[i] = (struct arasan_dma_desc) {
			.misc = skb_max_size()
		};
	}
	_rx_ring[RX_RING_SIZE - 1].misc |= DMA_RDES1_EOR;
	_rx_head = _rx_tail = 0;
	REG32_STORE(DMA_RECEIVE_BASE_ADDRESS, (uint32_t)_rx_ring);

	/* Setup registers */
	REG32_STORE(MAC_ADDRESS_CONTROL, 1);
	REG32_STORE(MAC_TRANSMIT_FIFO_ALMOST_FULL, (512 - 8));
	REG32_STORE(MAC_TRANSMIT_PACKET_START_THRESHOLD, 128);
	REG32_STORE(MAC_RECEIVE_PACKET_START_THRESHOLD, 64);

	reg = REG32_LOAD(MAC_RECEIVE_CONTROL);
	reg |= MAC_RECEIVE_CONTROL_STORE_AND_FORWARD;
	REG32_STORE(MAC_RECEIVE_CONTROL, reg);

	reg = REG32_LOAD(DMA_CONFIGURATION);
	reg |= DMA_CONFIGURATION_WAIT_FOR_DONE;
	REG32_STORE(DMA_CONFIGURATION, reg);

	/* schedule a link state check */
	//phy_start
	//napi_enable
	/* Enable interrupts */
	REG32_STORE(DMA_INTERRUPT_ENABLE,
			DMA_INTERRUPT_ENABLE_RECEIVE_DONE |
			DMA_INTERRUPT_ENABLE_TRANSMIT_DONE);

	/* Enable packet transmission */
	reg = REG32_LOAD(MAC_TRANSMIT_CONTROL);
	reg |= MAC_TRANSMIT_CONTROL_TRANSMIT_ENABLE;
	REG32_STORE(MAC_TRANSMIT_CONTROL, reg);

	/* Enable packet reception */
	reg = REG32_LOAD(MAC_RECEIVE_CONTROL);
	reg |= MAC_RECEIVE_CONTROL_RECEIVE_ENABLE;
	REG32_STORE(MAC_RECEIVE_CONTROL, reg);

	/* Start transmit and receive DMA */
	REG32_STORE(DMA_CONTROL,
			DMA_CONTROL_START_RECEIVE_DMA |
			DMA_CONTROL_START_TRANSMIT_DMA);

	_reg_dump();

	mii_id = phy_discovery();
	if (mii_id != -1) {
		printk("mii_id = %d\n", mii_id);
	} else {
		printk("phy is not found\n");
	}
	/* Setup etherdev */
        if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

	irq_attach(ARASAN_IRQ, arasan_int_handler, 0, nic, "arasan");

        nic->drv_ops = &arasan_drv_ops;

	return inetdev_register_dev(nic);
}

static struct periph_memory_desc arasan_mem = {
	.start = BASE_ADDR,
	.len   = 0x200
};

PERIPH_MEMORY_DEFINE(arasan_mem);
