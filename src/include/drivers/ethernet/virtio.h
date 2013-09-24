/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_ETHERNET_VIRTIO_H_
#define DRIVERS_ETHERNET_VIRTIO_H_

#include <assert.h>
#include <asm/io.h>
#include <stdint.h>
#include <net/netdevice.h>
#include <util/binalign.h>

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

/**
 * VirtIO IO Operations
 */
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
#define __VIRTIO_ANDIN(b)                                                \
	static inline void virtio_andin##b(uint##b##_t val, uint32_t reg,    \
			struct net_device *dev) {                                    \
		out##b(val & in##b(dev->base_addr + reg), dev->base_addr + reg); \
	}

__VIRTIO_LOAD(8)
__VIRTIO_LOAD(16)
__VIRTIO_LOAD(32)
__VIRTIO_STORE(8)
__VIRTIO_STORE(16)
__VIRTIO_STORE(32)
__VIRTIO_ORIN(8)
__VIRTIO_ANDIN(8)

#undef __VIRTIO_LOAD
#undef __VIRTIO_STORE
#undef __VIRTIO_ORIN

#define VRING_ALIGN_BOUND 4096

/**
 * VirtIO Register operations
 */
static inline void virtio_select_queue(uint16_t id, struct net_device *dev) {
	virtio_store16(id, VIRTIO_REG_QUEUE_SL, dev);
}

static inline void virtio_notify_queue(uint16_t id, struct net_device *dev) {
	virtio_store16(id, VIRTIO_REG_QUEUE_N, dev);
}

static inline uint16_t virtio_get_queue_size(struct net_device *dev) {
	return virtio_load16(VIRTIO_REG_QUEUE_SZ, dev);
}

static inline void virtio_set_queue_addr(void *addr, struct net_device *dev) {
	virtio_store32((uintptr_t)addr / VRING_ALIGN_BOUND, VIRTIO_REG_QUEUE_A, dev);
}

/**
 * VirtIO Ring Descriptor Table
 */
struct vring_desc {
	uint64_t addr;  /* Address (guest - physical) */
	uint32_t len;   /* Length */
	uint16_t flags; /* Flags */
	uint16_t next;  /* Next descriptor */
};

#define VRING_DESC_F_NEXT     0x1 /* Have next */
#define VRING_DESC_F_WRITE    0x2 /* Write-only buffer */
#define VRING_DESC_F_INDIRECT 0x4 /* The buffer contains a list of buffer
									 descriptors */

static void vring_desc_init(struct vring_desc *vrd,
		void *addr, unsigned int len, unsigned short flags) {
	assert(vrd != NULL);
	vrd->addr = (uint64_t)(uintptr_t)addr;
	vrd->len = len;
	vrd->flags = flags;
}

/**
 * VirtIO Ring Available
 */
struct vring_avail {
	uint16_t flags;         /* Flags */
	uint16_t idx;           /* Next ring id */
	uint16_t ring[];        /* Available rings */
	/* uint32_t used_event; -- placed at ring[-1] */
#define vring_used_event(vr) ((vr)->avail->ring[(vr)->num])
};

#define VRING_AVAIL_F_NO_INTERRUPT 0x1 /* Ignore interrupts while consumes a
										  descriptor from the available ring */

/**
 * VirtIO Rind Used
 */
struct vring_used_elem {
	uint32_t id;  /* Index of start of used descriptor chain */
	uint32_t len; /* Total length of the descriptor chain which was used */
};

struct vring_used {
	uint16_t flags;                /* Flags */
	uint16_t idx;                  /* Next ring id */
	struct vring_used_elem ring[]; /* Rings */
	/* uint16_t avail_event;       -- placed at ring[-1].id */
#define vring_avail_event(vr) ((vr)->used->ring[(vr)->num].id)
};

#define VRING_USED_F_NO_NOTIFY 0x1 /* Don't notify when the guest adds to
									  the available ring */

/**
 * VirtIO Ring
 */
struct vring {
	unsigned int num;          /* The number of descriptors */
	struct vring_desc *desc;   /* The list of actual descriptors */
	struct vring_avail *avail; /* A ring of available descriptor heads with
								  free-running index */
	/* char pad[];             -- Padding (without reference) */
	struct vring_used *used;   /* A ring of used descriptor heads with
								  free-running index */
};

static inline unsigned int vring_size(unsigned int num
		/* , unsigned long align */) {
	const unsigned long align = VRING_ALIGN_BOUND;
	const unsigned int vring_desc_sz = sizeof(struct vring_desc),
			vring_avail_sz = (3 + num) * sizeof(uint16_t),
			vring_used_sz = 3 * sizeof(uint16_t)
				+ num * sizeof(struct vring_used_elem);

	return binalign_bound(vring_desc_sz * num + vring_avail_sz, align)
			+ binalign_bound(vring_used_sz, align);
}

static inline void vring_init(struct vring *vr, unsigned int num,
		void *p/*, unsigned long align*/) {
	const unsigned long align = VRING_ALIGN_BOUND;

	assert(vr != NULL);
	vr->num = num;
	vr->desc = p;
	assert(binalign_check_bound((uintptr_t)vr->desc, align));
	vr->avail = p + num * sizeof(struct vring_desc);
	vr->used = (void *)binalign_bound((uintptr_t)&vr->avail->ring[num]
			+ sizeof(uint16_t), align);
	assert(binalign_check_bound((uintptr_t)vr->used, align));
}

#include <linux/compiler.h>
static inline void vring_push_desc(uint16_t id, struct vring *vr) {
	assert(vr != NULL);
	vr->avail->ring[vr->avail->idx % vr->num] = id;
	__barrier();
	++vr->avail->idx;
	__barrier();
}

/**
 * VirtIO Queue
 */
struct virtqueue {
	struct vring ring;       /* Ring of this queue */
	void *ring_data;         /* Allocated data for ring storage */
	uint16_t last_seen_used; /* Last seen used id */
	uint16_t next_free_desc; /* Next free descriptor id */
};

static inline int virtqueue_push_buff(void *buff, unsigned int len,
		int wronly, struct virtqueue *vq) {
	uint16_t desc_id;

	assert(vq != NULL);

	desc_id = vq->next_free_desc;
	vq->next_free_desc = (vq->next_free_desc + 1) % vq->ring.num;
	assert(vq->ring.desc[desc_id].addr == 0); /* overflow */

	vring_desc_init(&vq->ring.desc[desc_id], buff, len,
			wronly ? VRING_DESC_F_WRITE : 0);
	vring_push_desc(desc_id, &vq->ring);

	return 0;
}

#endif /* DRIVERS_ETHERNET_VIRTIO_H_ */
