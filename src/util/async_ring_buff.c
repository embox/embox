/**
 * @file
 * @brief Async ring buffer.
 *
 * @date 18.09.2012
 * @author Alexander Kalmuk
 */

#include <util/async_ring_buff.h>

int async_ring_buff_empty(struct async_ring_buff *buf) {
	return ring_buff_empty(&buf->buffer);
}

int async_ring_buff_available_space(struct async_ring_buff *buf) {
	return ring_buff_available_space(&buf->buffer);
}

int async_ring_buff_init(struct async_ring_buff *buf, size_t elem_size, int count, void *storage) {
	mutex_init(&buf->mutex);
	return ring_buff_init(&buf->buffer, elem_size, count, storage);
}

int async_ring_buff_enque(struct async_ring_buff *buf, void *elem, size_t cnt) {
	int len;

	mutex_lock(&buf->mutex);
	len = ring_buff_enque(&buf->buffer, elem, cnt);
	mutex_unlock(&buf->mutex);

	return len;
}

int async_ring_buff_deque(struct async_ring_buff *buf, void *elem, size_t cnt) {
	int len;

	mutex_lock(&buf->mutex);
	len = ring_buff_deque(&buf->buffer, elem, cnt);
	mutex_unlock(&buf->mutex);

	return len;
}
