/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIRTIO_VIRTIO_RING_H_
#define DRIVERS_VIRTIO_VIRTIO_RING_H_

#include <stddef.h>
#include <stdint.h>

/**
 * VirtIO Ring Descriptor Table
 */
struct vring_desc {
	uint64_t addr;  /* Address (guest - physical) */
	uint32_t len;   /* Length */
	uint16_t flags; /* Flags */
#define VRING_DESC_F_NEXT     0x1 /* Have next */
#define VRING_DESC_F_WRITE    0x2 /* Write-only buffer */
#define VRING_DESC_F_INDIRECT 0x4 /* The buffer contains a list of buffer
									 descriptors */
	uint16_t next;  /* Next descriptor */
};

extern void vring_desc_init(struct vring_desc *vrd,
		void *addr, uint32_t len, uint16_t flags);

/**
 * VirtIO Ring Available
 */
struct vring_avail {
	uint16_t flags;         /* Flags */
#define VRING_AVAIL_F_NO_INTERRUPT 0x1 /* Ignore interrupts while consumes a
										  descriptor from the available ring */
	uint16_t idx;           /* Next ring id */
	uint16_t ring[];        /* Available rings */
	/* uint32_t used_event; -- placed at ring[-1] */
#define vring_used_event(vr) ((vr)->avail->ring[(vr)->num])
};

/**
 * VirtIO Rind Used
 */
struct vring_used_elem {
	uint32_t id;  /* Index of start of used descriptor chain */
	uint32_t len; /* Total length of the descriptor chain which was used */
};

struct vring_used {
	uint16_t flags;                /* Flags */
#define VRING_USED_F_NO_NOTIFY 0x1 /* Don't notify when the guest adds to
									  the available ring */
	uint16_t idx;                  /* Next ring id */
	struct vring_used_elem ring[]; /* Rings */
	/* uint16_t avail_event;       -- placed at ring[-1].id */
#define vring_avail_event(vr) ((vr)->used->ring[(vr)->num].id)
};

/**
 * VirtIO Ring
 */
struct vring {
	uint16_t num;              /* The number of descriptors */
	struct vring_desc *desc;   /* The list of actual descriptors */
	struct vring_avail *avail; /* A ring of available descriptor heads with
								  free-running index */
	/* uint8_t pad[];          -- Padding (without reference) */
	struct vring_used *used;   /* A ring of used descriptor heads with
								  free-running index */
};

extern size_t vring_size(uint16_t num);
extern void vring_init(struct vring *vr, uint16_t num, void *mem);
extern void vring_push_desc(uint16_t id, struct vring *vr);

#endif /* DRIVERS_VIRTIO_VIRTIO_RING_H_ */
