/**
 * @file
 * @brief Nonblock ring buffer realization
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 * @author Vita Loginova
 */

#include <stddef.h>
#include <string.h>

#include <lib/libds/ring.h>
#include <lib/libds/ring_buff.h>

int ring_buff_alloc(struct ring_buff *buf, int cnt, void **ret) {
	struct ring *ring = &buf->ring;
	void *write_to = (char *)buf->storage + (ring->head * buf->elem_size);
	int can_write;

	if (ring_full(ring, buf->capacity)) {
		return 0;
	}

	*ret = write_to;

	can_write = ring_can_write(&buf->ring, buf->capacity, cnt);
	memset(write_to, 0, buf->elem_size * can_write);

	return ring_just_write(ring, buf->capacity, can_write);
}
