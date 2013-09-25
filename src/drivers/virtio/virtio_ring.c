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
#include <linux/compiler.h>
#include <stddef.h>
#include <stdint.h>
#include <util/binalign.h>

void vring_desc_init(struct vring_desc *vrd, void *addr,
		uint32_t len, uint16_t flags) {
	assert(vrd != NULL);

	vrd->addr = (uintptr_t)addr;
	vrd->len = len;
	vrd->flags = flags;
}

size_t vring_size(uint16_t num) {
	const size_t vring_desc_sz = sizeof(struct vring_desc),
			vring_avail_sz = (3 + num) * sizeof(uint16_t),
			vring_used_sz = 3 * sizeof(uint16_t)
				+ num * sizeof(struct vring_used_elem);

	return binalign_bound(vring_desc_sz * num + vring_avail_sz,
				VIRTIO_VRING_ALIGN)
			+ binalign_bound(vring_used_sz, VIRTIO_VRING_ALIGN);
}

void vring_init(struct vring *vr, uint16_t num, void *mem) {
	assert(vr != NULL);

	vr->num = num;
	vr->desc = mem;
	assert(binalign_check_bound((uintptr_t)vr->desc,
				VIRTIO_VRING_ALIGN));
	vr->avail = mem + num * sizeof(struct vring_desc);
	vr->used = (void *)binalign_bound(
			(uintptr_t)&vr->avail->ring[num] + sizeof(uint16_t),
			VIRTIO_VRING_ALIGN);
	assert(binalign_check_bound((uintptr_t)vr->used,
				VIRTIO_VRING_ALIGN));
}

void vring_push_desc(uint16_t id, struct vring *vr) {
	assert(vr != NULL);

	vr->avail->ring[vr->avail->idx % vr->num] = id;
	__barrier();
	++vr->avail->idx;
	__barrier();
}
