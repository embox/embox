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
#include <util/ring.h>
#include <util/ring_buff.h>

int ring_buff_get_cnt(struct ring_buff *buf) {
	return ring_data_size(&buf->ring, buf->capacity);
}

int ring_buff_get_space(struct ring_buff *buf) {
	return ring_room_size(&buf->ring, buf->capacity);
}

int ring_buff_fill_nulls(struct ring_buff *buf, int cnt) {
	struct ring *ring = &buf->ring;
	void *write_to = buf->storage + (ring->head * buf->elem_size);
	int rest = buf->capacity - ring->head;
	int can_write;

	if (ring_full(ring, buf->capacity)) {
		return 0;
	}

	can_write = ring_can_write(&buf->ring, buf->capacity, cnt);
	if (can_write > rest) {
		memset(write_to, 0, buf->elem_size * rest);
		memset(buf->storage, 0, buf->elem_size * (can_write - rest));
	} else {
		memset(write_to, 0, buf->elem_size * can_write);
	}

	return ring_just_write(ring, buf->capacity, can_write);
}

int ring_buff_enqueue(struct ring_buff *buf, void *from_buf, int cnt) {
	struct ring *ring = &buf->ring;
	void *write_to = buf->storage + (ring->head * buf->elem_size);
	int rest = buf->capacity - ring->head;
	int can_write;

	if (ring_full(ring, buf->capacity)) {
		return 0;
	}

	can_write = ring_can_write(&buf->ring, buf->capacity, cnt);
	if (can_write > rest) {
		memcpy(write_to, from_buf, buf->elem_size * rest);
		memcpy(buf->storage,
			(char *)from_buf + buf->elem_size * rest,
			buf->elem_size * (can_write - rest));
	} else {
		memcpy(write_to, from_buf, buf->elem_size * can_write);
	}

	return ring_just_write(ring, buf->capacity, can_write);
}

int ring_buff_dequeue(struct ring_buff *buf, void *into_buf, int cnt) {
	struct ring *ring = &buf->ring;
	void *read_from = buf->storage + (ring->tail * buf->elem_size);
	int rest = buf->capacity - ring->tail;
	int can_read;

	if (ring_empty(ring)) {
		return 0;
	}

	can_read = ring_can_read(ring, buf->capacity, cnt);
	if (can_read > rest) {
		memcpy(into_buf, read_from, buf->elem_size * rest);
		memcpy((char *)into_buf + buf->elem_size * rest,
			buf->storage,
			buf->elem_size * (can_read - rest));
	} else {
		memcpy(into_buf, read_from, buf->elem_size * can_read);
	}

	return ring_just_read(ring, buf->capacity, can_read);
}

int ring_buff_init(struct ring_buff *buf, size_t elem_size, int count, void *storage) {
	ring_init(&buf->ring);
	buf->capacity = count;
	buf->storage = storage;
	buf->elem_size = elem_size;

	return 0;
}
