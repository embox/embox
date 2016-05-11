/**
 * @file imx6_net.c
 * @brief iMX6 MAC-NET driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-05-11
 */

#include <assert.h>
#include <errno.h>

#include <hal/reg.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include <embox/unit.h>

#include <framework/mod/options.h>

#define NIC_BASE OPTION_GET(NUMBER, base_addr)

#define ENET_ECR  (NIC_BASE + 0x0024)
#define ENET_RCR  (NIC_BASE + 0x0084)
#define ENET_TCR  (NIC_BASE + 0x00C4)
#define MAC_LOW   (NIC_BASE + 0x00E4)
#define MAC_HI    (NIC_BASE + 0x00E8)
#define ENET_RDSR (NIC_BASE + 0x0180)
#define ENET_TDSR (NIC_BASE + 0x0184)
#define ENET_MRBR (NIC_BASE + 0x0188)

/* Various flags */
/* ENET_ECR */
#define ETHEREN  (1 << 1) /* Ethernet enable */
#define RESET    (1 << 0)

#define FRAME_LEN     1588
#define TX_BUF_FRAMES 256
#define TX_BUF_LEN    (FRAME_LEN * TX_BUF_FRAMES)
#define RX_BUF_FRAMES 256
#define RX_BUF_LEN    (FRAME_LEN * TX_BUF_FRAMES)

static uint8_t _tx_buf[TX_BUF_LEN] __attribute__ ((aligned(16)));
static uint8_t _rx_buf[RX_BUF_LEN] __attribute__ ((aligned(16)));

EMBOX_UNIT_INIT(imx6_net_init);
static int imx6_net_xmit(struct net_device *dev, struct sk_buff *skb) {
	skb_free(skb);
	return 0;
}

static int imx6_net_open(struct net_device *dev) {


	REG32_STORE(ENET_ECR, ETHEREN); /* Note: should be last init step */
	return 0;
}

static int imx6_net_set_macaddr(struct net_device *dev, const void *addr) {
	uint8_t *mac = (uint8_t *) addr;
	uint32_t mac_hi, mac_lo;

	assert(dev);
	assert(addr);

        assert(mac);

	mac_hi  = (mac[5] << 16) | (mac[4] << 8) | (mac[3] << 0);
	mac_lo  = (mac[2] << 16) | (mac[1] << 8) | (mac[0] << 0);

	REG32_STORE(MAC_LOW, mac_lo);
	REG32_STORE(MAC_HI, mac_hi);

	return 0;
}

static const struct net_driver imx6_net_drv_ops = {
	.xmit = imx6_net_xmit,
	.start = imx6_net_open,
	.set_macaddr = imx6_net_set_macaddr
};

static int imx6_net_init(void) {
	struct net_device *nic;

	if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

	nic->drv_ops = &imx6_net_drv_ops;

	REG32_STORE(ENET_ECR, RESET);

	/* Setup TX and RX buffers */
	assert((((uint32_t) &_tx_buf[0]) & 0xF) == 0);
	REG32_STORE(ENET_TDSR, ((uint32_t) &_tx_buf[0]));

	assert((((uint32_t) &_rx_buf[0]) & 0xF) == 0);
	REG32_STORE(ENET_RDSR, ((uint32_t) &_rx_buf[0]));

	assert((RX_BUF_LEN & 0xF) == 0);
	REG32_STORE(ENET_MRBR, RX_BUF_LEN);

	return inetdev_register_dev(nic);;
}
