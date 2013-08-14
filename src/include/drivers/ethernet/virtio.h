/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_ETHERNET_VIRTIO_H_
#define DRIVERS_ETHERNET_VIRTIO_H_

#include <asm/io.h>
#include <stdint.h>
#include <net/netdevice.h>

#define __VIRTIO_LOAD(b)                                   \
	static inline uint##b##_t virtio_load##b(uint32_t reg, \
			struct net_device *dev) {                      \
		return in##b(dev->base_addr + reg);                \
	}
#define __VIRTIO_STORE(b)                                             \
	static inline void virtio_store##b(uint##b##_t val, uint32_t reg, \
			struct net_device *dev) {                                 \
		out##b(val, dev->base_addr + reg);                            \
	}
#define __VIRTIO_ORIN(b)                                                 \
	static inline void virtio_orin##b(uint##b##_t val, uint32_t reg,     \
			struct net_device *dev) {                                    \
		out##b(val | in##b(dev->base_addr + reg), dev->base_addr + reg); \
	}

__VIRTIO_LOAD(8)
__VIRTIO_LOAD(16)
__VIRTIO_LOAD(32)
__VIRTIO_STORE(8)
__VIRTIO_STORE(16)
__VIRTIO_STORE(32)
__VIRTIO_ORIN(8)

#undef __VIRTIO_LOAD
#undef __VIRTIO_STORE
#undef __VIRTIO_ORIN

/**
 * VirtIO Device Registers
 */
#define VIRTIO_REG_DEVICE_F 0x00 /* Device features */
#define VIRTIO_REG_GUEST_F  0x04 /* Guest features */
#define VIRTIO_REG_QUEUE_A  0x08 /* Queue address */
#define VIRTIO_REG_QUEUE_SZ 0x0C /* Queue size */
#define VIRTIO_REG_QUEUE_SL 0x0E /* Queue select */
#define VIRTIO_REG_QUEUE_N  0x10 /* Queue notify */
#define VIRTIO_REG_DEVICE_S 0x12 /* Device status */
#define VIRTIO_REG_ISR_S    0x13 /* ISR status */

/**
 * VirtIO Network Device Registers
 */
#define VIRTIO_REG_NET_MAC(i) (0x14 + i) /* MAC address (i:0..5)*/
#define VIRTIO_REG_NET_STATUS 0x1A       /* Status (2 bytes)*/

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

/* Virtio ring descriptors: 16 bytes.  These can chain together via "next". */
struct vring_desc {
        /* Address (guest-physical). */
        __u64 addr;
        /* Length. */
        __u32 len;
        /* The flags as indicated above. */
        __u16 flags;
        /* We chain unused descriptors via this, too */
        __u16 next;
};

/* u32 is used here for ids for padding reasons. */
struct vring_used_elem {
        /* Index of start of used descriptor chain. */
        __u32 id;
        /* Total length of the descriptor chain which was used (written to) */
        __u32 len;
};

#define __ALIGN(x) (((x) + 4095) & ~4095)
static inline unsigned vring_size(unsigned int qsz) {
	return __ALIGN(sizeof(struct vring_desc) * qsz + sizeof(__u16) * (3 + qsz))
		+ __ALIGN(sizeof(struct vring_used_elem) * qsz + 3 * sizeof(__u16));
}
#undef __ALIGN

#endif /* DRIVERS_ETHERNET_VIRTIO_H_ */
