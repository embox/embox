/**
 * @file
 * @brief Nonblock ring buffer realization
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <string.h>
#include <util/ring_buff.h>

int ring_buff_get_cnt(struct ring_buff *buf) {
	return buf->cnt;
}

int ring_buff_available_space(struct ring_buff *buf) {
	return buf->capacity - buf->cnt;
}

int ring_buff_empty(struct ring_buff *buf) {
	return !buf->cnt;
}

int ring_buff_enque(struct ring_buff *buf, void *elem) {
	char *buf_pool = buf->storage;

	if (buf->cnt >= buf->capacity) {
		return -1;
	}

	buf->cnt++;
	memcpy(buf_pool + (buf->p_write * buf->elem_size), (const void *) elem,
			buf->elem_size);
	buf->p_write = (buf->p_write + 1) % buf->capacity;

	return 0;
}

int ring_buff_deque(struct ring_buff *buf, void *elem) {
	char *buf_pool = buf->storage;
	void *ret = buf_pool + (buf->p_read * buf->elem_size);

	if (buf->cnt == 0) {
		return -1;
	}

	buf->cnt--;
	buf->p_read = (buf->p_read + 1) % buf->capacity;
	memcpy(elem, ret, buf->elem_size);

	return 0;
}

int ring_buff_init(struct ring_buff *buf, size_t elem_size, int count, void *storage) {
	buf->capacity  = count;
	buf->cnt = 0;
	buf->p_write  = 0;
	buf->p_read  = 0;
	buf->storage  = storage;
	buf->elem_size = elem_size;

	return 0;
}
