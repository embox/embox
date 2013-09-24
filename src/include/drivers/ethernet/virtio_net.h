/**
 * @file
 *
 * @date Sep 10, 2013
 * @author: Anton Bondarev
 */

#ifndef VIRTIO_NET_H_
#define VIRTIO_NET_H_

#include <asm/io.h>
#include <stdint.h>

/**
 * VirtIO Network Device Registers
 */
#define VIRTIO_REG_NET_MAC(i) (0x14 + i) /* MAC address (i:0..5)*/
#define VIRTIO_REG_NET_STATUS 0x1A       /* Status (2 bytes)*/

/**
 * VirtIO Network Device Queues
 */
#define VIRTIO_NET_QUEUE_RX   0 /* Receive queue */
#define VIRTIO_NET_QUEUE_TX   1 /* Transmission queue */
#define VIRTIO_NET_QUEUE_CTRL 2 /* Control queue (optional) */

/**
 * VirtIO Device Status
 */
#define VIRTIO_CONFIG_S_ACKNOWLEDGE 0x01 /* Guest OS has found the device and
											recognized it as a valid virtio
											device */
#define VIRTIO_CONFIG_S_DRIVER      0x02 /* Guest OS knows how to drive the
											device */
#define VIRTIO_CONFIG_S_DRIVER_OK   0x04 /* Driver is set up and ready to drive
											the device */
#define VIRTIO_CONFIG_S_FAILED      0x80 /* Something went wront */

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
	uint8_t gso_type;     /* Type of Generic segmentation
							 offload (GSO) */
	uint16_t hdr_len;     /* Header length */
	uint16_t gso_size;    /* Size of GSO */
	uint16_t csum_start;  /* Calculate checksum from this place */
	uint16_t csum_offset; /* Size of this place */
};

#define VIRTIO_NET_HDR_F_NEEDS_CSUM 0x1

#define VIRTIO_NET_HDR_GSO_NONE 0
#define VIRTIO_NET_HDR_GSO_TPV4 1
#define VIRTIO_NET_HDR_GSO_UDP  3
#define VIRTIO_NET_HDR_GSO_TPV6 4
#define VIRTIO_NET_HDR_GSO_ECN  0x80

#endif /* VIRTIO_NET_H_ */
