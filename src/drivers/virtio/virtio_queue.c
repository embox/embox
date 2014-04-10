/**
 * @file
 * @brief
 *
 * @date 10.09.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/virtio/virtio.h>
#include <drivers/virtio/virtio_ring.h>
#include <drivers/virtio/virtio_queue.h>
#include <errno.h>
#include <mem/sysmalloc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int virtqueue_create(struct virtqueue *vq, uint16_t q_id,
		unsigned long base_addr) {
	uint16_t queue_sz;
	size_t ring_sz;
	void *ring_mem;

	assert(vq != NULL);

	virtio_select_queue(q_id, base_addr);

	queue_sz = virtio_get_queue_size(base_addr);
	ring_sz = vring_size(queue_sz);

	ring_mem = sysmemalign(VIRTIO_VRING_ALIGN, ring_sz);
	if (ring_mem == NULL) {
		return -ENOMEM;
	}
	memset(ring_mem, 0, ring_sz);

	vq->id = q_id;
	vring_init(&vq->ring, queue_sz, ring_mem);
	vq->ring_mem = ring_mem;
	vq->last_seen_used = vq->next_free_desc = 0;

	virtio_set_queue_addr(ring_mem, base_addr);

	return 0;
}

void virtqueue_destroy(struct virtqueue *vq,
		unsigned long base_addr) {
	assert(vq != NULL);

	virtio_select_queue(vq->id, base_addr);
	virtio_set_queue_addr(0, base_addr);

	sysfree(vq->ring_mem);
}

struct vring_desc * virtqueue_alloc_desc(struct virtqueue *vq) {
	struct vring_desc *vrd;

	assert(vq != NULL);
	assert(vq->ring.desc != NULL);

	vrd = &vq->ring.desc[vq->next_free_desc];
	if (vrd->addr != 0) {
		return NULL;
	}

	vq->next_free_desc = (vq->next_free_desc + 1) % vq->ring.num;

	return vrd;
}
