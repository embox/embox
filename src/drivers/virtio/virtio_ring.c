/**
 * @file
 *
 * @date Sep 10, 2013
 * @author: Ilia Vaprol
 */
#include <assert.h>
#include <drivers/virtio.h>
#include <linux/compiler.h>
#include <util/binalign.h>

static inline void vring_desc_init(struct vring_desc *vrd,
		void *addr, unsigned int len, unsigned short flags) {
	assert(vrd != NULL);
	vrd->addr = (uint64_t)(uintptr_t)addr;
	vrd->len = len;
	vrd->flags = flags;
}

void vring_push_desc(uint16_t id, struct vring *vr) {
	assert(vr != NULL);
	vr->avail->ring[vr->avail->idx % vr->num] = id;
	__barrier();
	++vr->avail->idx;
	__barrier();
}

int virtqueue_push_buff(void *buff, unsigned int len, int wronly, struct virtqueue *vq) {
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


unsigned int vring_size(unsigned int num
		/* , unsigned long align */) {
	const unsigned long align = VRING_ALIGN_BOUND;
	const unsigned int vring_desc_sz = sizeof(struct vring_desc),
			vring_avail_sz = (3 + num) * sizeof(uint16_t),
			vring_used_sz = 3 * sizeof(uint16_t)
				+ num * sizeof(struct vring_used_elem);

	return binalign_bound(vring_desc_sz * num + vring_avail_sz, align)
			+ binalign_bound(vring_used_sz, align);
}

void vring_init(struct vring *vr, unsigned int num, void *p) {
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


