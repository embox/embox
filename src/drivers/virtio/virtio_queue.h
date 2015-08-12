/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIRTIO_VIRTIO_QUEUE_H_
#define DRIVERS_VIRTIO_VIRTIO_QUEUE_H_

#include <stdint.h>

/**
 * Prototypes
 */
struct vring_desc;

/**
 * VirtIO Queue
 */
struct virtqueue {
	uint16_t id;             /* Identifier */
	struct vring ring;       /* Ring of this queue */
	void *ring_mem;          /* Allocated data for ring storage */
	uint16_t last_seen_used; /* Last seen used id */
	uint16_t next_free_desc; /* Next free descriptor id */
};

extern int virtqueue_create(struct virtqueue *vq, uint16_t q_id,
		unsigned long base_addr);
extern void virtqueue_destroy(struct virtqueue *vq,
		unsigned long base_addr);
extern struct vring_desc * virtqueue_alloc_desc(struct virtqueue *vq);

#endif /* DRIVERS_VIRTIO_VIRTIO_QUEUE_H_ */
