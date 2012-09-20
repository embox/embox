/**
 * @file
 * @brief Async ring buffer.
 *
 * @date 18.09.2012
 * @author Alexander Kalmuk
 */

#include <util/async_ring_buff.h>

size_t async_ring_get_cnt(struct async_ring_buff *buf) {
	return ring_buff_get_cnt(&buf->buffer);
}

size_t async_ring_buff_get_space(struct async_ring_buff *buf) {
	return ring_buff_get_space(&buf->buffer);
}

int async_ring_buff_init(struct async_ring_buff *buf, size_t elem_size, size_t count, void *storage) {
	mutex_init(&buf->mutex);
	return ring_buff_init(&buf->buffer, elem_size, count, storage);
}

size_t async_ring_buff_push(struct async_ring_buff *buf, void *elem, size_t cnt) {
	int len;

	mutex_lock(&buf->mutex);
	len = ring_buff_push(&buf->buffer, elem, cnt);
	mutex_unlock(&buf->mutex);

	return len;
}

size_t async_ring_buff_pop(struct async_ring_buff *buf, void *elem, size_t cnt) {
	int len;

	mutex_lock(&buf->mutex);
	len = ring_buff_pop(&buf->buffer, elem, cnt);
	mutex_unlock(&buf->mutex);

	return len;
}
