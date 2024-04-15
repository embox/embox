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

int ring_buff_get_cnt(struct ring_buff *buf) {
	return ring_data_size(&buf->ring, buf->capacity);
}

int ring_buff_get_space(struct ring_buff *buf) {
	return ring_room_size(&buf->ring, buf->capacity);
}

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

static int __ring_buff_enqueue(struct ring_buff *buf,
	void *into_buf, void *from_buf, int cnt) {
	struct ring *ring = &buf->ring;
	int can_write;

	can_write = ring_can_write(ring, buf->capacity, cnt);
	memcpy(into_buf, from_buf, buf->elem_size * can_write);
	ring_just_write(ring, buf->capacity, can_write);

	return can_write;
}

int ring_buff_enqueue(struct ring_buff *buf, void *from_buf, int cnt) {
	struct ring *ring = &buf->ring;
	void *write_to = (char *)buf->storage + (ring->head * buf->elem_size);
	int written, rest;

	if (ring_full(ring, buf->capacity)) {
		return 0;
	}

	written = __ring_buff_enqueue(buf, write_to, from_buf, cnt);

	if (ring_full(ring, buf->capacity)) {
		return written;
	}

	rest = cnt - written;
	if (rest) {
		written += __ring_buff_enqueue(buf, buf->storage,
			(char *)from_buf + buf->elem_size * written, rest);
	}

	return written;
}

static int __ring_buff_dequeue(struct ring_buff *buf,
	void *into_buf, void *from_buf, int cnt) {
	struct ring *ring = &buf->ring;
	int can_read;

	can_read = ring_can_read(ring, buf->capacity, cnt);
	memcpy(into_buf, from_buf, buf->elem_size * can_read);
	ring_just_read(ring, buf->capacity, can_read);

	return can_read;
}

int ring_buff_dequeue(struct ring_buff *buf, void *into_buf, int cnt) {
	struct ring *ring = &buf->ring;
	void *read_from = (char *)buf->storage + (ring->tail * buf->elem_size);
	int read, rest;

	if (ring_empty(ring)) {
		return 0;
	}

	read = __ring_buff_dequeue(buf, into_buf, read_from, cnt);

	if (ring_empty(ring)) {
		return read;
	}

	rest = cnt - read;
	if (rest) {
		read += __ring_buff_dequeue(buf, (char *)into_buf + buf->elem_size * read,
			buf->storage, rest);
	}

	return read;
}

int ring_buff_init(struct ring_buff *buf, size_t elem_size, int count, void *storage) {
	ring_init(&buf->ring);
	buf->capacity = count;
	buf->storage = storage;
	buf->elem_size = elem_size;

	return 0;
}
