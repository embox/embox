/**
 * @file
 * @brief
 *
 * @date 25.09.13
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_ETHERNET_VIRTIO_NET_H_
#define DRIVERS_ETHERNET_VIRTIO_NET_H_

#include <drivers/virtio/virtio.h>
#include <drivers/virtio/virtio_io.h>
#include <drivers/virtio/virtio_queue.h>
#include <net/netdevice.h>
#include <stdint.h>

/**
 * VirtIO Network Device Registers
 */
#define VIRTIO_REG_NET_MAC(i) (0x14 + i) /* MAC address (i:0..5) */
#define VIRTIO_REG_NET_STATUS 0x1A       /* Status (2 bytes) */

/**
 * VirtIO Network Device Queues
 */
#define VIRTIO_NET_QUEUE_RX   0 /* Receive queue */
#define VIRTIO_NET_QUEUE_TX   1 /* Transmission queue */
#define VIRTIO_NET_QUEUE_CTRL 2 /* Control queue (optional) */

/**
 * VirtIO Network Device Feature Bits
 */
#define VIRTIO_NET_F_CSUM           0x000001 /* Device handles pkts w/ partial
												csum */
#define VIRTIO_NET_F_GUEST_CSUM     0x000002 /* Guest handles pkts w/ partial
												csum */
#define VIRTIO_NET_F_MAC            0x000020 /* Device has given MAC address */
#define VIRTIO_NET_F_GSO            0x000040 /* Device handles pkts w/ any GSO
												type */
#define VIRTIO_NET_F_GUEST_TSO4     0x000080 /* Guest can receive TSOv4 */
#define VIRTIO_NET_F_GUEST_TSO6     0x000100 /* Guest can receive TSOv6 */
#define VIRTIO_NET_F_GUEST_ECN      0x000200 /* Guest can receive TSO w/ ECN */
#define VIRTIO_NET_F_GUEST_UFO      0x000400 /* Guest can receive UFO */
#define VIRTIO_NET_F_HOST_TSO4      0x000800 /* Device can receive TSOv4 */
#define VIRTIO_NET_F_HOST_TSO6      0x001000 /* Device can receive TSOv6 */
#define VIRTIO_NET_F_HOST_ECN       0x002000 /* Device can receive TSO w/ ECN */
#define VIRTIO_NET_F_HOST_UFO       0x004000 /* Device can receive UFO */
#define VIRTIO_NET_F_MRG_RXBUF      0x008000 /* Guest can merge receive buff */
#define VIRTIO_NET_F_STATUS         0x010000 /* Configuration status field
												is available */
#define VIRTIO_NET_F_CTRL_VQ        0x020000 /* Control channel is available */
#define VIRTIO_NET_F_CTRL_RX        0x040000 /* Control channel RX mode
												support */
#define VIRTIO_NET_F_CTRL_VLAN      0x080000 /* Control channel VLAN
												filtering */
#define VIRTIO_NET_F_CTRL_RX_EXTRA  0x100000 /* Extra RX mode control support */
#define VIRTIO_NET_F_GUEST_ANNOUNCE 0x200000 /* Guest can send gratuitous
												pkts */
#define VIRTIO_NET_F_MQ             0x400000 /* Device supports Receive Flow
												Steering */
#define VIRTIO_NET_F_CTRL_MAC_ADDR  0x800000 /* Set MAC address */

/**
 * VirtIO Network Device Status
 */
#define VIRTIO_NET_S_LINK_UP  0x1 /* Link is up */
#define VIRTIO_NET_S_ANNOUNCE 0x2 /* Announcement is needed */

/**
 * VirtIO Network Packet Header
 */
struct virtio_net_hdr {
	uint8_t flags;        /* Flags */
#define VIRTIO_NET_HDR_F_NEEDS_CSUM 0x1
	uint8_t gso_type;     /* Type of Generic segmentation
							 offload (GSO) */
#define VIRTIO_NET_HDR_GSO_NONE 0x00
#define VIRTIO_NET_HDR_GSO_TPV4 0x01
#define VIRTIO_NET_HDR_GSO_UDP  0x03
#define VIRTIO_NET_HDR_GSO_TPV6 0x04
#define VIRTIO_NET_HDR_GSO_ECN  0x80
	uint16_t hdr_len;     /* Header length */
	uint16_t gso_size;    /* Size of GSO */
	uint16_t csum_start;  /* Calculate checksum from this place */
	uint16_t csum_offset; /* Size of this place */
};

/**
 * VirtIO Operation Definitions For Network Module
 */
#define virtio_net_has_feature(feature, dev)      \
	virtio_has_feature(feature, dev->base_addr)
#define virtio_net_set_feature(feature, dev)      \
	virtio_set_feature(feature, dev->base_addr)
#define virtio_net_select_queue(q_id, dev)        \
	virtio_select_queue(q_id, dev->base_addr)
#define virtio_net_notify_queue(q_id, dev)        \
	virtio_notify_queue(q_id, dev->base_addr)
#define virtio_net_get_queue_size(dev)            \
	virtio_get_queue_size(dev->base_addr)
#define virtio_net_set_queue_addr(q_addr, dev)    \
	virtio_set_queue_addr(q_addr, dev->base_addr)
#define virtio_net_reset(dev)                     \
	virtio_reset(dev->base_addr)
#define virtio_net_add_status(status, dev)        \
	virtio_add_status(status, dev->base_addr)
#define virtio_net_del_status(status, dev)        \
	virtio_del_status(status, dev->base_addr)
#define virtio_net_get_isr_status(dev)            \
	virtio_get_isr_status(dev->base_addr)

/**
 * VirtIO Queue Operation Definitions For Network Module
 */
#define virtqueue_net_create(vq, q_id, dev)    \
	virtqueue_create(vq, q_id, dev->base_addr)
#define virtqueue_net_destroy(vq, dev)         \
	virtqueue_destroy(vq, dev->base_addr)

/**
 * Virtio Network MAC Operations
 */
static inline uint8_t virtio_net_get_mac(int i,
		struct net_device *dev) {
	return virtio_load8(VIRTIO_REG_NET_MAC(i), dev->base_addr);
}

static inline void virtio_net_set_mac(uint8_t mac_i, int i,
		struct net_device *dev) {
	virtio_store8(mac_i, VIRTIO_REG_NET_MAC(i), dev->base_addr);
}

/**
 * VirtIO Network Status Operations
 */
static inline uint16_t virtio_net_get_status(
		struct net_device *dev) {
	return virtio_load16(VIRTIO_REG_NET_STATUS, dev->base_addr);
}

#endif /* DRIVERS_ETHERNET_VIRTIO_NET_H_ */
