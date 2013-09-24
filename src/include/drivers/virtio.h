/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_ETHERNET_VIRTIO_H_
#define DRIVERS_ETHERNET_VIRTIO_H_


#include <stdint.h>
#include <asm/io.h>
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



#define VRING_ALIGN_BOUND 4096



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
#undef __VIRTIO_ANDIN

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
	assert(binalign_check_bound((uintptr_t)addr, VRING_ALIGN_BOUND));
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

/**
 * VirtIO Queue
 */
struct virtqueue {
	struct vring ring;       /* Ring of this queue */
	void *ring_data;         /* Allocated data for ring storage */
	uint16_t last_seen_used; /* Last seen used id */
	uint16_t next_free_desc; /* Next free descriptor id */
};


extern void vring_desc_init(struct vring_desc *vrd,
		void *addr, unsigned int len, unsigned short flags);

extern unsigned int vring_size(unsigned int num);

extern void vring_init(struct vring *vr, unsigned int num, void *p);

extern void vring_push_desc(uint16_t id, struct vring *vr);

extern int virtqueue_push_buff(void *buff, unsigned int len, int wronly,
		struct virtqueue *vq);

#endif /* DRIVERS_ETHERNET_VIRTIO_H_ */
