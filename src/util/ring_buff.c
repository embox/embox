/**
 * @file
 * @brief Nonblock ring buffer realization
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <string.h>
#include <kernel/thread/sched_lock.h>
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

int ring_buff_enque(struct ring_buff *buf, void *elem, size_t cnt) {
	int space, rest;
	void *write_to = (char*)buf->storage + (buf->p_write * buf->elem_size);
	rest = buf->capacity - buf->p_write;

	if (buf->cnt >= buf->capacity) {
		return -1;
	}

	if ((space = ring_buff_available_space(buf)) < cnt) {
		cnt = space;
	}

	if (cnt > rest) {
		memcpy(write_to, elem, buf->elem_size * rest);
		memcpy(buf->storage, (char *)elem + buf->elem_size * rest, buf->elem_size * (cnt - rest));
		buf->p_write = cnt - rest;
	} else {
		memcpy(write_to, elem, buf->elem_size * cnt);
		buf->p_write = (buf->p_write + cnt) % buf->capacity;
	}

	buf->cnt += cnt;

	return cnt;
}

int ring_buff_deque(struct ring_buff *buf, void *elem, size_t cnt) {
	int rest = buf->capacity - buf->p_read;
	void *read_from = (char*)buf->storage + (buf->p_read * buf->elem_size);

	if (buf->cnt == 0) {
		return -1;
	}

	if (buf->cnt < cnt) {
		cnt = buf->cnt;
	}

	buf->cnt -= cnt;

	if (cnt > rest) {
		memcpy(elem, read_from, buf->elem_size * rest);
		memcpy((char *)elem + buf->elem_size * rest, buf->storage, buf->elem_size * (cnt - rest));
		buf->p_read = cnt - rest;
	} else {
		memcpy(elem, read_from, buf->elem_size * cnt);
		buf->p_read = (buf->p_read + cnt) % buf->capacity;
	}

	return cnt;
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
